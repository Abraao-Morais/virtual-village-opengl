#include "skybox.h"

#include <iostream>
#include <stdexcept>

#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

namespace
{
    constexpr float skyboxVertices[] = {
        // Posições dos 36 vértices do cubo
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};
}

Skybox::Skybox(const std::array<std::string, 6> &faces, const char *vertexShaderPath, const char *fragmentShaderPath) : skyboxShader(vertexShaderPath, fragmentShaderPath)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    glBindVertexArray(0);

    cubemapTexture = loadCubemap(faces);
}

Skybox::~Skybox()
{
    if (cubemapTexture != 0)
        glDeleteTextures(1, &cubemapTexture);

    if (VBO != 0)
        glDeleteBuffers(1, &VBO);

    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
}

GLuint Skybox::loadCubemap(const std::array<std::string, 6> &faces)
{
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // Não inverter verticalmente as imagens do cubemap.
    stbi_set_flip_vertically_on_load(false);

    int referenceWidth = 0;
    int referenceHeight = 0;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned int i = 0; i < faces.size(); ++i)
    {
        int width = 0;
        int height = 0;
        int channels = 0;

        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &channels, STBI_rgb_alpha);

        if (!data)
        {
            std::cerr << "Erro ao carregar face do cubemap: " << faces[i] << '\n';

            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            glDeleteTextures(1, &textureID);

            throw std::runtime_error("Nao foi possivel carregar uma imagem da skybox.");
        }

        if (width != height || (i > 0 && (width != referenceWidth || height != referenceHeight)))
        {
            std::cerr << "As faces do cubemap devem ser quadradas " << "e ter o mesmo tamanho: " << faces[i] << '\n';

            stbi_image_free(data);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            glDeleteTextures(1, &textureID);

            throw std::runtime_error("Dimensoes invalidas nas imagens da skybox.");
        }

        referenceWidth = width;
        referenceHeight = height;

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

void Skybox::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    skyboxShader.use();

    GLint program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    glm::mat4 rotationOnlyView = glm::mat4(glm::mat3(view));

    GLint viewLocation = glGetUniformLocation(program, "view");
    GLint projectionLocation = glGetUniformLocation(program, "projection");
    GLint skyboxLocation = glGetUniformLocation(program, "skybox");

    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(rotationOnlyView));

    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform1i(skyboxLocation, 0);

    GLint previousDepthFunc = GL_LESS;
    GLboolean previousDepthMask = GL_TRUE;
    GLboolean wasCullEnabled = glIsEnabled(GL_CULL_FACE);

    glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunc);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &previousDepthMask);

    glDisable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glDepthMask(previousDepthMask);
    glDepthFunc(previousDepthFunc);

    if (wasCullEnabled)
        glEnable(GL_CULL_FACE);
}