#include "ground.h"

Ground::Ground() {
    // Plano de 200 x 200 unidades.
    // Os seis vértices formam dois triângulos.
    const float vertices[] = {
        -100.0f, 0.0f, -100.0f,
         100.0f, 0.0f, -100.0f,
         100.0f, 0.0f,  100.0f,

        -100.0f, 0.0f, -100.0f,
         100.0f, 0.0f,  100.0f,
        -100.0f, 0.0f,  100.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    // A posição de cada vértice ocupa 3 floats.
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        3 * sizeof(float), (void*)0
    );

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Ground::~Ground() {
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
    }

    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
    }
}

void Ground::draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}