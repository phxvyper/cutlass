//
// Created by Ashley on 11/26/2018.
//

#ifndef CUTLASS_TRANSFORM_H
#define CUTLASS_TRANSFORM_H

#include <common.h>

class Transform {
public:
    glm::vec3 worldPosition;
    glm::vec3 localRotation;
    glm::vec3 localScale;

    Transform();
    glm::mat4 LocalTransform() const;
    glm::mat4 WorldTransform() const;
};

#endif //CUTLASS_GAMEOBJECT_H