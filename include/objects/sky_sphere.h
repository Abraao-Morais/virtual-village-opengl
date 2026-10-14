#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <vector>
#include <cmath>
#include <stdexcept>
#include <string>

#include "stb_image.h"

class SkySphere {
    public:
        GLuint VAO = 0;
        GLuint VBO = 0;
        GLuint EBO = 0;
        GLuint texture = 0;
        GLsizei indexCount = 0;

        bool loadTexture(const std::string& path) {
            stbi_set_flip_vertically_on_load(false);

            int width, height, channels;
            unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

            if (!data)
                return false;

            GLenum format = channels == 4 ? GL_RGBA : GL_RGB;

            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            stbi_image_free(data);
            glBindTexture(GL_TEXTURE_2D, 0);

            return true;
        }

        void create(float radius = 100.0f, unsigned int segments = 64, unsigned int rings = 32) {
            std::vector<float> vertices;
            std::vector<unsigned int> indices;

            for (unsigned int y = 0; y <= rings; ++y) {
                float v = float(y) / rings;
                float phi = glm::pi<float>() * v;

                for (unsigned int x = 0; x <= segments; ++x) {
                    float u = float(x) / segments;
                    float theta = 2.0f * glm::pi<float>() * u;

                    float px = -radius * std::sin(phi) * std::cos(theta);
                    float py =  radius * std::cos(phi);
                    float pz =  radius * std::sin(phi) * std::sin(theta);

                    vertices.push_back(px);
                    vertices.push_back(py);
                    vertices.push_back(pz);
                }
            }

            for (unsigned int y = 0; y < rings; ++y) {
                for (unsigned int x = 0; x < segments; ++x) {
                    unsigned int a = y * (segments + 1) + x;
                    unsigned int b = a + segments + 1;

                    indices.push_back(a);
                    indices.push_back(b);
                    indices.push_back(a + 1);

                    indices.push_back(b);
                    indices.push_back(b + 1);
                    indices.push_back(a + 1);
                }
            }

            indexCount = static_cast<GLsizei>(indices.size());

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
            glEnableVertexAttribArray(0);
            glBindVertexArray(0);
        }

        void draw() const {
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }

        void cleanup(){
            if (texture != 0) glDeleteTextures(1, &texture);
            if (EBO != 0) glDeleteBuffers(1, &EBO);
            if (VBO != 0) glDeleteBuffers(1, &VBO);
            if (VAO != 0) glDeleteVertexArrays(1, &VAO);

            texture = 0;
            EBO = 0;
            VBO = 0;
            VAO = 0;
        }
};
