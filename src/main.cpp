#include "common.h"
#include "cutlass.h"
#include <stb_image.h>

int width = 1024;
int height = 576;
const char gameTitle[8] = "Cutlass";
constexpr float fixedTimestep(1 / 60.0f);
std::map<int, ButtonFlags> buttonMap = {
        {GLFW_KEY_ESCAPE,         CUT_ESC},
        {GLFW_KEY_SPACE,          CUT_JUMP},
        {GLFW_KEY_W,              CUT_MOVE_FORWARD},
        {GLFW_KEY_A,              CUT_MOVE_LEFT},
        {GLFW_KEY_S,              CUT_MOVE_BACK},
        {GLFW_KEY_D,              CUT_MOVE_RIGHT},
        {GLFW_KEY_LEFT_CONTROL,   CUT_DUCK},
        {GLFW_MOUSE_BUTTON_LEFT,  CUT_ATTACK},
        {GLFW_MOUSE_BUTTON_RIGHT, CUT_ATTACK_ALT},
};

GLFWwindow *window;
GameState currentState;
GameState previousState;
glm::vec2 deltaScroll;
glm::vec2 deltaMousePos;
glm::vec2 mousePos;
bool mouseSet = false;

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    std::cout << "key: " << key << std::endl;
}

void MouseCallback(GLFWwindow *window, double xpos, double ypos) {
    if (!mouseSet) {
        mousePos = glm::vec2((float) xpos, (float) ypos);
        mouseSet = true;
    }

    deltaMousePos = glm::vec2(xpos - mousePos.x, ypos - mousePos.y);
    mousePos = glm::vec2((float) xpos, (float) ypos);
}

void ScrollCallback(GLFWwindow *window, double xpos, double ypos) {
    deltaScroll = glm::vec2(xpos, ypos);
}

void FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    // fix for an issue on mac OS where resizing the window doesnt automatically
    // update our viewport.
    glViewport(0, 0, width, height);
}

void UpdateInput() {
    currentState.mousePos = mousePos;
    currentState.deltaMousePos = deltaMousePos;
    currentState.deltaScroll = deltaScroll;

    // start with no buttons pressed/cleared flags
    ButtonFlags buttonFlags = CUT_BUTTON_NONE;
    for (auto const&[key, val] : buttonMap) {
        if (glfwGetKey(window, key) == GLFW_PRESS) {
            buttonFlags |= val;
        }
    }

    currentState.oldButtonFlags = currentState.buttonFlags;
    currentState.buttonFlags = buttonFlags;
}

void Update(float time, float deltaTime) {
    if (currentState.buttonFlags & CUT_ESC) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else if (currentState.buttonFlags & CUT_ATTACK) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    currentState.player.FixedUpdate(currentState, time, deltaTime);

    for (auto it = currentState.objects.begin(); it != currentState.objects.end(); ++it) {
        it->second.FixedUpdate(currentState, time, deltaTime);
    }
}

GameState InterpolateState(GameState const &current, GameState const &previous, double alpha) {

    /*
     * state interpolation is implementation specific, so this boiler plate doesn't do much other than interpolate
     * entities and the camera
     */

    GameState integratedState(previous);

    for (auto it = integratedState.objects.begin(); it != integratedState.objects.end(); ++it)
    {
        // we only interpolate if our new state contains the same entity.
        // the map's key acts as a UID for the entity.
        if (current.objects.count(it->first))
        {
            // interpolate the iterated ent towards the current ent
            auto currentEnt = current.objects.at(it->first);
            auto ent = it->second;

            ent.worldPosition = glm::mix(ent.worldPosition, currentEnt.worldPosition, alpha);
            ent.localScale = glm::mix(ent.localScale, currentEnt.localScale, alpha);
            ent.localRotation = glm::mix(ent.localRotation, currentEnt.localRotation, alpha);

            it->second = ent;
        }
    }

    /*
     * interpolate player
     */
    auto player = integratedState.player;
    if (player.position != previous.player.position) player.position = glm::mix(player.position, previousState.player.position, alpha);
    if (player.rotation != previous.player.rotation) player.rotation = glm::mix(player.rotation, previousState.player.rotation, alpha);
    integratedState.player = player;

    /*
     * interpolate player's camera
     */
    auto camera = integratedState.player.camera;
    if (camera.position != previous.player.camera.position) camera.position = glm::mix(camera.position, previous.player.camera.position, alpha);
    if (camera.rotation != previous.player.camera.rotation) camera.rotation = glm::mix(camera.position, previous.player.camera.rotation, alpha);
    integratedState.player.camera = camera;

    return integratedState;
}

void Render(GameState renderState) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 view = renderState.player.camera.View();
    const glm::mat4 projection = renderState.player.camera.Projection();

    for (auto it = renderState.objects.begin(); it != renderState.objects.end(); ++it) {
        auto obj = it->second;
        RenderMesh(&obj.mesh, &obj.material, obj.LocalTransform(), obj.WorldTransform(), view, projection);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main() {

    int result = CUT_NO_ERROR;
    try {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

        window = glfwCreateWindow(width, height, gameTitle, NULL, NULL);
        if (window == NULL) {
            std::cout << "Failed to create GLFW window" << std::endl;
            throw CUT_ERROR_NO_GLFW;
        }

        /*
         * to facilitate retina displays, get the real resolution that we should render to
         */
        glfwGetFramebufferSize(window, &width, &height);
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            throw CUT_ERROR_NO_GLAD;
        }

        glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
        glfwSetScrollCallback(window, ScrollCallback);
        glfwSetCursorPosCallback(window, MouseCallback);
        glfwSetKeyCallback(window, KeyCallback);

        FramebufferSizeCallback(window, width, height);
        glEnable(GL_DEPTH_TEST);

        std::cout << "GL VENDOR = " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "GL RENDERER = " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "GL VERSION = " << glGetString(GL_VERSION) << std::endl;

        // some asset stuff
        stbi_set_flip_vertically_on_load(true);
        Texture container;
        Texture awesomeface;

        LoadTexture(&container, "assets/container.jpg", GL_RGB, GL_REPEAT, GL_NEAREST);
        LoadTexture(&awesomeface, "assets/awesomeface.png", GL_RGBA, GL_REPEAT, GL_NEAREST);

        auto basicShader = Shader("shader/basic.vertex.glsl", "shader/basic.fragment.glsl");
        basicShader.SetInt("u_texture", 0);
        basicShader.SetInt("u_texture1", 1);

        /*
         * default world clip
         */
        auto worldClip = CreateGenericWorldClip();
        GameObject clip;
        clip.mesh = worldClip;
        clip.material.texture[0] = container;
        clip.material.texture[1] = awesomeface;
        clip.material.shader = basicShader;
        currentState.PushObject(clip);

        currentState.player.position = glm::vec3(0.0f, 0.0f, 0.0f);
        currentState.player.rotation = 0.0f;
        currentState.player.hull = glm::vec3(49.0f, 83.0f, 49.0f);
        currentState.player.duckHull = glm::vec3(49.0f, 69.0f, 49.0f);
        currentState.player.eyeHeight = 65.0f;
        currentState.player.duckEyeHeight = 51.0f;

        currentState.player.camera.rotation.x = 15.0f;

        // finally our game loops begins!
        float accumulator = 0.0f;
        float currentTime = glfwGetTime();

        while (!glfwWindowShouldClose(window)) {
            float deltaTime = glfwGetTime() - currentTime;
            currentTime = glfwGetTime();
            accumulator += deltaTime;

            UpdateInput();

            while (accumulator > fixedTimestep) {
                accumulator -= fixedTimestep;

                previousState = currentState;
                Update(currentTime, fixedTimestep);
            }

            float alpha = accumulator / fixedTimestep;
            GameState renderState = InterpolateState(currentState, previousState, alpha);

            Render(renderState);
        }
    }
    catch (int error) {
        std::cout << "There was a Cutlass Error (" << error << ")" << std::endl;
        result = error;
    }

    glfwTerminate();
    return result;
}