#include "objects/pumpkin.h"
#include "objects/mesh_utils.h" 

#include <vector>
#include <cmath>

using MeshUtils::addTriangle;
using MeshUtils::addQuad;
using MeshUtils::vertexCount;

namespace {
    std::vector<float> vertices;
    constexpr float PI = 3.14159265358979323846f;

    void addPumpkinBody() {
        // Criando gomos aproximados da abóbora em formato esférico/cilíndrico com curvas
        int segments = 8;
        float radius = 1.0f;
        float height = 1.4f;

        for (int i = 0; i < segments; ++i) {
            float theta1 = (float)i / segments * 2.0f * PI;
            float theta2 = (float)(i + 1) / segments * 2.0f * PI;

            float x1 = cos(theta1) * radius;
            float z1 = sin(theta1) * radius;
            float x2 = cos(theta2) * radius;
            float z2 = sin(theta2) * radius;

            // Corpo abaulado (engrossa no meio y = 0.7)
            addQuad(vertices, 
                x1 * 0.8f, 0.0f, z1 * 0.8f,
                x2 * 0.8f, 0.0f, z2 * 0.8f,
                x2 * 1.1f, height * 0.5f, z2 * 1.1f,
                x1 * 1.1f, height * 0.5f, z1 * 1.1f
            );
            addQuad(vertices, 
                x1 * 1.1f, height * 0.5f, z1 * 1.1f,
                x2 * 1.1f, height * 0.5f, z2 * 1.1f,
                x2 * 0.7f, height, z2 * 0.7f,
                x1 * 0.7f, height, z1 * 0.7f
            );
        }
    }

    void addStem() {
        // Cabinho verde da abóbora no topo
        addQuad(vertices, -0.15f, 1.4f, -0.15f,  0.15f, 1.4f, -0.15f,  0.1f, 1.8f, -0.1f, -0.1f, 1.8f, -0.1f);
        addQuad(vertices,  0.15f, 1.4f,  0.15f, -0.15f, 1.4f,  0.15f, -0.1f, 1.8f,  0.1f,  0.1f, 1.8f,  0.1f);
        addQuad(vertices, -0.15f, 1.4f,  0.15f, -0.15f, 1.4f, -0.15f, -0.1f, 1.8f, -0.1f, -0.1f, 1.8f,  0.1f);
        addQuad(vertices,  0.15f, 1.4f, -0.15f,  0.15f, 1.4f,  0.15f,  0.1f, 1.8f,  0.1f,  0.1f, 1.8f, -0.1f);
    }

    void addEyes() {
        // Olhos triangulares maléficos estilo Halloween (voltados para a frente +Z)
        addTriangle(vertices, -0.4f, 0.9f, 1.05f, -0.6f, 0.5f, 1.05f, -0.3f, 0.5f, 1.05f);
        addTriangle(vertices,  0.4f, 0.9f, 1.05f,  0.3f, 0.5f, 1.05f,  0.6f, 0.5f, 1.05f);
    }

    void addMouth() {
        // Boca sorridente denteada clássica de Jack-o'-lantern
        addQuad(vertices, -0.5f, 0.3f, 1.05f,  0.5f, 0.3f, 1.05f,  0.4f, 0.1f, 1.05f, -0.4f, 0.1f, 1.05f);
    }
}

Pumpkin::Pumpkin() {
    bodyStart = vertexCount(vertices);
    addPumpkinBody();
    bodyCount = vertexCount(vertices) - bodyStart;

    stemStart = vertexCount(vertices);
    addStem();
    stemCount = vertexCount(vertices) - stemStart;

    eyesStart = vertexCount(vertices);
    addEyes();
    eyesCount = vertexCount(vertices) - eyesStart;

    mouthStart = vertexCount(vertices);
    addMouth();
    mouthCount = vertexCount(vertices) - mouthStart;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Pumpkin::~Pumpkin() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Pumpkin::draw() const {
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, bodyStart, bodyCount);
    glDrawArrays(GL_TRIANGLES, stemStart, stemCount);
    glDrawArrays(GL_TRIANGLES, eyesStart, eyesCount);
    glDrawArrays(GL_TRIANGLES, mouthStart, mouthCount);

    glBindVertexArray(0);
}

void Pumpkin::draw(GLint colorLocation) const {
    glBindVertexArray(VAO);

    // Cor Laranja vibrante para o corpo da abóbora
    glUniform3f(colorLocation, 1.0f, 0.5f, 0.0f);
    glDrawArrays(GL_TRIANGLES, bodyStart, bodyCount);

    // Cor Verde escuro para o cabinho
    glUniform3f(colorLocation, 0.1f, 0.5f, 0.1f);
    glDrawArrays(GL_TRIANGLES, stemStart, stemCount);

    // Cor Amarela brilhante para os olhos e boca (como se estivessem acesos por dentro)
    glUniform3f(colorLocation, 1.0f, 0.9f, 0.0f);
    glDrawArrays(GL_TRIANGLES, eyesStart, eyesCount);
    glDrawArrays(GL_TRIANGLES, mouthStart, mouthCount);

    glBindVertexArray(0);
}