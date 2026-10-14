#include "objects/house.h"

#include "objects/mesh_utils.h"
#include <vector>

using MeshUtils::addTriangle;
using MeshUtils::addQuad;
using MeshUtils::vertexCount;

namespace {
    std::vector<float> vertices;

    void addWalls() {
        addQuad(vertices, -1,0,1, 1,0,1, 1,2,1, -1,2,1);
        addQuad(vertices, 1,0,-1, -1,0,-1, -1,2,-1, 1,2,-1);

        addQuad(vertices, -1,0,-1, -1,0, 1, -1,2, 1, -1,2,-1);
        addQuad(vertices, 1,0, 1,  1,0,-1,  1,2,-1,  1,2, 1);

        addQuad(vertices, -1,2, 1,  1,2, 1,  1,2,-1, -1,2,-1);
        addQuad(vertices, -1,0,-1,  1,0,-1,  1,0, 1, -1,0, 1);
    }

    void addRoof() {
        addQuad(vertices, -1.4f, 1.7f,  1.4f, 0.0f, 3.0f,  1.0f, 0.0f, 3.0f, -1.0f, -1.4f, 1.7f, -1.4f);
        addQuad(vertices, 1.4f, 1.7f,  1.4f, 0.0f, 3.0f,  1.0f, 0.0f, 3.0f, -1.0f, 1.4f, 1.7f, -1.4f);

        addTriangle(vertices, -1.4f, 1.7f, 1.4f, 1.4f, 1.7f, 1.4f, 0.0f, 3.0f, 1.0f);
        addTriangle(vertices, 1.4f, 1.7f, -1.4f, -1.4f, 1.7f, -1.4f, 0.0f, 3.0f, -1.0f);
    }

    void addDoor() {
        addQuad(vertices, -0.25f,0,1.025f, 0.25f,0,1.025f, 0.25f,1.2f,1.025f, -0.25f,1.2f,1.025f);
    }
}

House::House() {
    wallsStart = vertexCount(vertices);
    addWalls();
    wallsCount = vertexCount(vertices) - wallsStart;

    roofStart = vertexCount(vertices);
    addRoof();
    roofCount = vertexCount(vertices) - roofStart;

    doorStart = vertexCount(vertices);
    addDoor();
    doorCount = vertexCount(vertices) - doorStart;

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

House::~House() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void House::draw() const {
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, wallsStart, wallsCount);
    glDrawArrays(GL_TRIANGLES, roofStart, roofCount);
    glDrawArrays(GL_TRIANGLES, doorStart, doorCount);

    glBindVertexArray(0);
}

void House::draw(GLint colorLocation) const {
    glBindVertexArray(VAO);

    glUniform3f(colorLocation, 0.5255f, 0.4784f, 0.4275f);
    glDrawArrays(GL_TRIANGLES, wallsStart, wallsCount);

    glUniform3f(colorLocation, 0.2588f, 0.2431f, 0.2823f);
    glDrawArrays(GL_TRIANGLES, roofStart, roofCount);

    glUniform3f(colorLocation, 0.2078f, 0.2f, 0.2274f);
    glDrawArrays(GL_TRIANGLES, doorStart, doorCount);

    glBindVertexArray(0);
}

