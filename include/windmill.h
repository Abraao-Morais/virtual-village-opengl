#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Windmill {
public:
    Windmill();
    ~Windmill();

    void draw() const;
    void draw(GLint colorLocation) const;
    void drawBlades() const;
    void drawBlades(GLint colorLocation) const;

private:
    GLuint VAO = 0;
    GLuint VBO = 0;

    GLint wallsStart = 0;
    GLint wallsCount = 0;

    GLint roofStart = 0;
    GLint roofCount = 0;

    GLint doorStart = 0;
    GLint doorCount = 0;

    GLint bladesStart;
    GLint bladesCount;
};