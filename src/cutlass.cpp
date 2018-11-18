//
// Created by Ashley Horton on 11/9/18.
//
#include "cutlass.h"
#include <stb_image.h>

Mesh::Mesh() {
    glGenVertexArrays(1, &this->vertex_array);
    glGenBuffers(1, &this->vertex_buffer);
    glGenBuffers(1, &this->element_buffer);
};

void Mesh::UpdateGL() {
    glBindVertexArray(this->vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, this->vertex_count * sizeof(Vertex), this->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->index_count * sizeof(int), this->indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);
};

GameObject::GameObject() : localScale(1.0f) {

};

glm::mat4 GameObject::LocalTransform() const {
    glm::mat4 transform;

    transform = glm::rotate(transform, glm::radians(this->localRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(this->localRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(this->localRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, this->localScale);

    return transform;
};

glm::mat4 GameObject::WorldTransform() const {
    glm::mat4 transform;

    transform = glm::translate(transform, this->worldPosition);

    return transform;
};

Camera::Camera(): Camera(90.0f, 16.0f / 9.0f, 0.01f, 4096.0f) {}

Camera::Camera(float fieldOfView, float aspectRatio, float nearClip, float farClip)
        : fov(fieldOfView), aspectRatio(aspectRatio), near(nearClip), far(farClip) {}

glm::mat4 Camera::View() const {
    glm::mat4 view;
    view = glm::rotate(view, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::translate(view, glm::vec3(-position.x, -position.y, position.z));
    return view;
}

glm::mat4 Camera::Projection() const {
    return glm::perspective(glm::radians(this->fov), this->aspectRatio, this->near, this->far);
}

Player::Player() {

}

void Player::FixedUpdate(GameState &state, float time, float deltaTime) {

    /*
     * null-canceling style movement (opposing movements cancel each other out)
     */
    auto wishDir = glm::vec3(0.0f, 0.0f, 0.0f);
    if (state.buttonFlags & CUT_MOVE_FORWARD) {
        wishDir.z += 1.0f;
    }
    if (state.buttonFlags & CUT_MOVE_BACK) {
        wishDir.z -= 1.0f;
    }
    if (state.buttonFlags & CUT_MOVE_RIGHT) {
        wishDir.x += 1.0f;
    }
    if (state.buttonFlags & CUT_MOVE_LEFT) {
        wishDir.x -= 1.0f;
    }

    /*
     * keep magnitude uniform
     */
    glm::normalize(wishDir);

    /*
     * move 50 units a second
     */
    this->position += wishDir * 400.0f * deltaTime;

    /*
     * update camera position and rotation based on player's position and rotation
     */
    auto cameraPos = this->position + glm::vec3(0.0f, this->eyeHeight, 0.0f);
    auto cameraLook = glm::vec3(
            this->camera.rotation.x,
            this->rotation,
            0.0f
    );

    this->camera.position = cameraPos;
    this->camera.rotation = cameraLook;
}

GameState::GameState() : buttonFlags(CUT_BUTTON_NONE), oldButtonFlags(CUT_BUTTON_NONE), player() {

}

void GameState::PushObject(GameObject const &object) {
    this->objects.insert(std::pair<int, GameObject>(++objectCount, object));
}

Error LoadTexture(Texture *texture, const char *filepath, GLenum format, GLint wrapping, GLint filtering) {
    Error error = CUT_NO_ERROR;
    glGenTextures(1, &texture->ID);
    glBindTexture(GL_TEXTURE_2D, texture->ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);

    int width, height, channels;
    stbi_uc *data = stbi_load(filepath, &width, &height, &channels, 0);
    if (data)
    {
        std::cout << "Loaded texture: " << filepath << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        std::cout << "Error loading texture: " << filepath << std::endl;
        error = CUT_ASSETS_TEXTURE_NOT_READ;
    }

    stbi_image_free(data);
    return error;
}

void RenderMesh(const Mesh *mesh, const Material *material, glm::mat4 local, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material->texture[0].ID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material->texture[1].ID);

    material->shader.Use();

    auto modelView = view * model * local;
    auto modelViewProjection = projection * modelView;

    material->shader.SetMat4("model_view_projection", modelViewProjection);
    material->shader.SetMat4("model_view", modelView);

    // render elements associated with mesh
    glBindVertexArray(mesh->vertex_array);
    glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);

    // clear our dependency to the mesh's vertex array to ensure that future changes
    // don't affect this vertex array.
    glBindVertexArray(0);
}

Mesh CreateGenericWorldClip()
{
    Vertex vertices[] = {
            { glm::vec3(1024.0f, 0.0f, 1024.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(32.0f, 32.0f) }, // top right
            { glm::vec3(1024.0f, 0.0f, -1024.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(32.0f, 0.0f) }, // bottom right
            { glm::vec3(-1024.0f, 0.0f, -1024.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) }, // bottom left
            { glm::vec3(-1024.0f, 0.0f, 1024.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 32.0f) }  // top left
    };
    int indices[] = {
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
    };

    Mesh mesh;
    mesh.index_count = ArrayLength(indices);
    mesh.vertex_count = ArrayLength(vertices);
    mesh.indices = (int *)indices;
    mesh.vertices = (Vertex *)vertices;
    mesh.UpdateGL();
    return mesh;
}

Mesh CreateTriangleMesh()
{
    Vertex vertices[] = {
            { glm::vec3(-32.0f, -32.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) }, // bottom left
            { glm::vec3(32.0f, -32.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) }, // bottom right
            { glm::vec3(0.0f, 32.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec2(0.5f, 1.0f) } // top
    };
    int indices[] = {
            0, 1, 2
    };

    Mesh mesh;
    mesh.index_count = ArrayLength(indices);
    mesh.vertex_count = ArrayLength(vertices);
    mesh.indices = (int *)indices;
    mesh.vertices = (Vertex *)vertices;
    mesh.UpdateGL();
    return mesh;
}

Mesh CreateSquareMesh()
{
    Vertex vertices[] = {
            { glm::vec3(32.0f,  32.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f) }, // top right
            { glm::vec3(32.0f, -32.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) }, // bottom right
            { glm::vec3(-32.0f, -32.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f) }, // bottom left
            { glm::vec3(-32.0f,  32.0f, 0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) }  // top left
    };
    int indices[] = {  // note that we start from 0!
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
    };

    Mesh mesh;
    mesh.index_count = ArrayLength(indices);
    mesh.vertex_count = ArrayLength(vertices);
    mesh.indices = (int *)indices;
    mesh.vertices = (Vertex *)vertices;
    mesh.UpdateGL();
    return mesh;
}