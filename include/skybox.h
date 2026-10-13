#pragma once

#include <array>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"

class Skybox {
public:
    Skybox(const std::array<std::string, 6>& faces, const char* vertexShaderPath, const char* fragmentShaderPath);
    ~Skybox();

    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;

    void draw(const glm::mat4& view, const glm::mat4& projection) const;

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint cubemapTexture = 0;

    Shader skyboxShader;

    GLuint loadCubemap(
        const std::array<std::string, 6>& faces
    );
};