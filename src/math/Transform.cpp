//
// Created by Ashley on 11/26/2018.
//

#include "Transform.h"

Transform::Transform() : localScale(1.0f) {

};

glm::mat4 Transform::LocalTransform() const {
    glm::mat4 transform;

    transform = glm::rotate(transform, glm::radians(this->localRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(this->localRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(this->localRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, this->localScale);

    return transform;
};

glm::mat4 Transform::WorldTransform() const {
    glm::mat4 transform;

    transform = glm::translate(transform, this->worldPosition);

    return transform;
};