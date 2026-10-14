#pragma once

#include <glad/glad.h>

// Lápide de pedra procedural, com base, placa arredondada e cruz frontal.
class Gravestone {
public:
    Gravestone();
    ~Gravestone();

    Gravestone(const Gravestone&) = delete;
    Gravestone& operator=(const Gravestone&) = delete;

    // Desenho para a cena normal (com cores).
    void draw(GLint colorLocation) const;

    // Desenho usado no passe de sombras.
    void draw() const;

private:
    GLuint VAO = 0;
    GLuint VBO = 0;

    GLint baseStart = 0, baseCount = 0;
    GLint stoneStart = 0, stoneCount = 0;
    GLint detailStart = 0, detailCount = 0;
};
