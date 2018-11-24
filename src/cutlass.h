//
// Created by Ashley Horton on 11/9/18.
//

#ifndef CUTLASS_CUTLASS_H
#define CUTLASS_CUTLASS_H

#include "common.h"
#include "shader.h"

/*
 * forward declarations
 */
 struct Vertex;
 struct Texture;
 struct Material;
 class Mesh;
 class GameObject;
 class Camera;
 class Player;
 class GameState;

/*
 * flags
 */
enum ButtonFlags {
    CUT_BUTTON_NONE = 0,
    CUT_ESC = 1 << 0,
    CUT_MOVE_FORWARD = 1 << 1,
    CUT_MOVE_RIGHT = 1 << 2,
    CUT_MOVE_LEFT = 1 << 3,
    CUT_MOVE_BACK = 1 << 4,
    CUT_JUMP = 1 << 5,
    CUT_DUCK = 1 << 6,
    CUT_ATTACK = 1 << 7,
    CUT_ATTACK_ALT = 1 << 8,

    CUT_LOOK_UP = 1 << 9,
    CUT_LOOK_LEFT = 1 << 10,
    CUT_LOOK_RIGHT = 1 << 11,
    CUT_LOOK_DOWN = 1 << 12
};

/*
 * graphics stuff
 */
struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texcoord;
};

struct Texture {
    GLuint ID;
};

struct Material {
    Shader shader;
    Texture texture[16];
};

class Mesh {
public:
    GLuint vertex_array;
    GLuint element_buffer;
    GLuint vertex_buffer;

    unsigned  int index_count;
    unsigned int vertex_count;
    int *indices;
    Vertex *vertices;

    Mesh();
    void UpdateGL();
};


/*
 * base game stuff
 */
class GameObject {
public:
    glm::vec3 worldPosition;
    glm::vec3 localRotation;
    glm::vec3 localScale;
    Mesh mesh;
    Material material;

    GameObject();
    virtual void FixedUpdate(GameState &state, float time, float deltaTime) {};
    glm::mat4 LocalTransform() const;
    glm::mat4 WorldTransform() const;
};

class Camera {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    float fov;
    float aspectRatio;
    float near;
    float far;

    Camera();
    Camera(float fieldOfView, float aspectRatio, float nearClip, float farClip);
    glm::mat4 View() const;
    glm::mat4 Projection() const;
};

class Player {
public:
    glm::vec3 position;
    float rotation;

    glm::vec3 hull;
    glm::vec3 duckHull;
    float eyeHeight;
    float duckEyeHeight;

    Camera camera;

    Player();
    void FixedUpdate(GameState &state, float time, float deltaTime);
};

class GameState {
private:
    int objectCount;
public:
    glm::vec2 mousePos;
    glm::vec2 deltaMousePos;
    glm::vec2 deltaScroll;

    ButtonFlags buttonFlags;
    ButtonFlags oldButtonFlags;

    std::map<int, GameObject> objects;
    Player player;

    GameState();
    void PushObject(GameObject const& object);
};

/*
 * actual game stuff
 */


/*
 * utility functions
 */
Error LoadTexture(Texture *texture, const char *filepath, GLenum format, GLint wrapping, GLint filtering);

void RenderMesh(const Mesh *mesh, const Material *material, glm::mat4 local, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
Mesh CreateGenericWorldClip();
Mesh CreateTriangleMesh();
Mesh CreateSquareMesh();

#endif //CUTLASS_CUTLASS_H
