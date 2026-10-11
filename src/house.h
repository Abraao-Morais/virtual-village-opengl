
#pragma once

#include <glad/glad.h>

class House {
public:
    House();
    ~House();

    House(const House&) = delete;
    House& operator=(const House&) = delete;

    void draw(GLint colorLocation) const;

private:
    GLuint VAO = 0;
    GLuint VBO = 0;

    GLint wallsStart = 0;
    GLint wallsCount = 0;

    GLint roofStart = 0;
    GLint roofCount = 0;

    GLint doorStart = 0;
    GLint doorCount = 0;

    GLint windowsStart = 0;
    GLint windowsCount = 0;
};
