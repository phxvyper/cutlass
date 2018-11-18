//
// Created by Ashley Horton on 11/9/18.
//

#ifndef CUTLASS_COMMON_H
#define CUTLASS_COMMON_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#define CUT_FILE_READ "r"
#define CUT_FILE_TRUNC "w"
#define CUT_FILE_APPEND "a"

#define ArrayLength(array) (sizeof(array))/(sizeof(array[0]))

enum Error {
    CUT_NO_ERROR = 0,
    CUT_ERROR_NO_GLFW = 0x1000,
    CUT_ERROR_NO_GLAD = 0x1001,
    CUT_ERROR_NO_FILE = 0x1002,
    CUT_ERROR_SHADER_FAIL = 0x2000,
    CUT_ERROR_PROGRAM_FAIL = 0x2001,
    CUT_ERROR_SHADER_FILE_NOT_READ = 0x2002,
    CUT_ASSETS_TEXTURE_NOT_READ = 0x3000,
};

template<class T>
inline T operator~(T a) { return (T) ~(int) a; }

template<class T>
inline T operator|(T a, T b) { return (T) ((int) a | (int) b); }

template<class T>
inline T operator&(T a, T b) { return (T) ((int) a & (int) b); }

template<class T>
inline T operator^(T a, T b) { return (T) ((int) a ^ (int) b); }

template<class T>
inline T &operator|=(T &a, T b) { return (T &) ((int &) a |= (int) b); }

template<class T>
inline T &operator&=(T &a, T b) { return (T &) ((int &) a &= (int) b); }

template<class T>
inline T &operator^=(T &a, T b) { return (T &) ((int &) a ^= (int) b); }

#endif //CUTLASS_COMMON_H
