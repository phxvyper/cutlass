//
// Created by Ashley Horton on 11/9/18.
//

#include "shader.h"

#include "common.h"

#include <sstream>
#include <fstream>

const Shader *currentShader;

Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vertexFile;
    std::ifstream fragmentFile;

    vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vertexFile.open(vertexPath);
        fragmentFile.open(fragmentPath);

        std::stringstream vertexFileStream, fragmentFileStream;

        vertexFileStream << vertexFile.rdbuf();
        fragmentFileStream << fragmentFile.rdbuf();

        vertexFile.close();
        fragmentFile.close();

        vertexCode = vertexFileStream.str();
        fragmentCode = fragmentFileStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "Failed to read vertex or fragment shader files: " << vertexPath << ", " << fragmentPath << std::endl;
        throw CUT_ERROR_SHADER_FILE_NOT_READ;
    }

    const char *vShader = vertexCode.c_str();
    const char *fShader = fragmentCode.c_str();

    GLuint vertex, fragment;
    int success;
    char infoLog[512];

    // compile our vertex code after reading in from the vertex file path
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, (const char *const *)&vShader, NULL);
    glCompileShader(vertex);

    // log any errors if there were any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "Vertex shader compilation failed. Log:\n" << infoLog << std::endl;
    }

    // compile our fragment code after reading in from the fragment file path
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, (const char *const *)&fShader, NULL);
    glCompileShader(fragment);

    // log any errors if there were any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "Fragment shader compilation failed. Log:\n" << infoLog << std::endl;
    }

    this->ID = glCreateProgram();
    glAttachShader(this->ID, vertex);
    glAttachShader(this->ID, fragment);
    glLinkProgram(this->ID);
    // log linking errors if there were any
    glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
        std::cout << "Program linking failed. Log:\n" << infoLog << std::endl;
    }

    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
void Shader::Use() const
{
    glUseProgram(this->ID);
    currentShader = this;
}
void Shader::SetBool(const std::string &name, bool value) const
{
    this->SetInt(name, value);
}
void Shader::SetInt(const std::string &name, int value) const
{
    glUseProgram(this->ID);
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
    if (currentShader) glUseProgram(currentShader->ID);
}
void Shader::SetFloat(const std::string &name, float value) const
{
    glUseProgram(this->ID);
    glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value);
    if (currentShader) glUseProgram(currentShader->ID);
}
void Shader::SetVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(this->ID, name.c_str()), 1, &value[0]);
}
void Shader::SetVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(this->ID, name.c_str()), x, y);
}
void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(this->ID, name.c_str()), 1, &value[0]);
}
void Shader::SetVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(this->ID, name.c_str()), x, y, z);
}
void Shader::SetVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(this->ID, name.c_str()), 1, &value[0]);
}
void Shader::SetVec4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(this->ID, name.c_str()), x, y, z, w);
}
void Shader::SetMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::SetMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
GLuint Shader::GetUniformLocation(const std::string &name) const
{
    return glGetUniformLocation(this->ID, name.c_str());
}
