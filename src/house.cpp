
#include "house.h"

#include <vector>

namespace {
    std::vector<float> vertices;

    void addTriangle(
        float ax, float ay, float az,
        float bx, float by, float bz,
        float cx, float cy, float cz
    ) {
        vertices.insert(vertices.end(), {
            ax, ay, az,
            bx, by, bz,
            cx, cy, cz
        });
    }

    void addQuad(
        float ax, float ay, float az,
        float bx, float by, float bz,
        float cx, float cy, float cz,
        float dx, float dy, float dz
    ) {
        addTriangle(ax, ay, az, bx, by, bz, cx, cy, cz);
        addTriangle(ax, ay, az, cx, cy, cz, dx, dy, dz);
    }

    GLint vertexCount() {
        return static_cast<GLint>(vertices.size() / 3);
    }

    void addWalls() {
        // Frente e traseira
        addQuad(-1,0, 1,  1,0, 1,  1,2, 1, -1,2, 1);
        addQuad( 1,0,-1, -1,0,-1, -1,2,-1,  1,2,-1);

        // Laterais
        addQuad(-1,0,-1, -1,0, 1, -1,2, 1, -1,2,-1);
        addQuad( 1,0, 1,  1,0,-1,  1,2,-1,  1,2, 1);

        // Topo e base
        addQuad(-1,2, 1,  1,2, 1,  1,2,-1, -1,2,-1);
        addQuad(-1,0,-1,  1,0,-1,  1,0, 1, -1,0, 1);
    }

    void addRoof() {
        // Duas águas do telhado
        addQuad(-1,2, 1.1f,  0,3, 1.1f,
                 0,3,-1.1f, -1,2,-1.1f);

        addQuad( 0,3, 1.1f,  1,2, 1.1f,
                 1,2,-1.1f,  0,3,-1.1f);

        // Triângulos da frente e de trás
        addTriangle(-1,2,1.02f, 1,2,1.02f, 0,3,1.02f);
        addTriangle( 1,2,-1.02f, -1,2,-1.02f, 0,3,-1.02f);
    }

    void addDoor() {
        // Porta castanha na fachada frontal
        addQuad(-0.25f,0,1.025f,
                 0.25f,0,1.025f,
                 0.25f,1.2f,1.025f,
                -0.25f,1.2f,1.025f);
    }

    void addWindows() {
        // Janela esquerda
        addQuad(-0.8f,0.9f,1.025f,
                -0.4f,0.9f,1.025f,
                -0.4f,1.4f,1.025f,
                -0.8f,1.4f,1.025f);

        // Janela direita
        addQuad(0.4f,0.9f,1.025f,
                0.8f,0.9f,1.025f,
                0.8f,1.4f,1.025f,
                0.4f,1.4f,1.025f);
    }
}

House::House() {
    // Paredes
    wallsStart = vertexCount();
    addWalls();
    wallsCount = vertexCount() - wallsStart;

    // Telhado
    roofStart = vertexCount();
    addRoof();
    roofCount = vertexCount() - roofStart;

    // Porta
    doorStart = vertexCount();
    addDoor();
    doorCount = vertexCount() - doorStart;

    // Janelas
    windowsStart = vertexCount();
    addWindows();
    windowsCount = vertexCount() - windowsStart;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        3 * sizeof(float), nullptr
    );
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

House::~House() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void House::draw(GLint colorLocation) const {
    glBindVertexArray(VAO);

    // Paredes bege
    glUniform3f(colorLocation, 0.82f, 0.72f, 0.55f);
    glDrawArrays(GL_TRIANGLES, wallsStart, wallsCount);

    // Telhado vermelho
    glUniform3f(colorLocation, 0.65f, 0.12f, 0.10f);
    glDrawArrays(GL_TRIANGLES, roofStart, roofCount);

    // Porta castanha
    glUniform3f(colorLocation, 0.30f, 0.14f, 0.06f);
    glDrawArrays(GL_TRIANGLES, doorStart, doorCount);

    // Janelas azuis
    glUniform3f(colorLocation, 0.25f, 0.65f, 0.90f);
    glDrawArrays(GL_TRIANGLES, windowsStart, windowsCount);

    glBindVertexArray(0);
}

