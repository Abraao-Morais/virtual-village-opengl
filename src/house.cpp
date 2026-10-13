
#include "house.h"

#include <vector>

namespace {
    std::vector<float> vertices;

    void addTriangle(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz) {
        vertices.insert(vertices.end(), {
            ax, ay, az,
            bx, by, bz,
            cx, cy, cz
        });
    }

    void addQuad(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz, float dx, float dy, float dz) {
        addTriangle(ax, ay, az, bx, by, bz, cx, cy, cz);
        addTriangle(ax, ay, az, cx, cy, cz, dx, dy, dz);
    }

    GLint vertexCount() {
        return static_cast<GLint>(vertices.size() / 3);
    }

    void addWalls() {
        addQuad(-1,0, 1,  1,0, 1,  1,2, 1, -1,2, 1);
        addQuad( 1,0,-1, -1,0,-1, -1,2,-1,  1,2,-1);

        addQuad(-1,0,-1, -1,0, 1, -1,2, 1, -1,2,-1);
        addQuad( 1,0, 1,  1,0,-1,  1,2,-1,  1,2, 1);

        addQuad(-1,2, 1,  1,2, 1,  1,2,-1, -1,2,-1);
        addQuad(-1,0,-1,  1,0,-1,  1,0, 1, -1,0, 1);
    }

    void addRoof() {
        addQuad(
            -1.4f, 1.7f,  1.4f,
            0.0f, 3.0f,  1.0f,
            0.0f, 3.0f, -1.0f,
            -1.4f, 1.7f, -1.4f
        );

        addQuad(
            1.4f, 1.7f,  1.4f,
            0.0f, 3.0f,  1.0f,
            0.0f, 3.0f, -1.0f,
            1.4f, 1.7f, -1.4f
        );

        addTriangle(
            -1.4f, 1.7f, 1.4f,
            1.4f, 1.7f, 1.4f,
            0.0f, 3.0f, 1.0f
        );

        addTriangle(
            1.4f, 1.7f, -1.4f,
            -1.4f, 1.7f, -1.4f,
            0.0f, 3.0f, -1.0f
        );
    }

    void addDoor() {
        addQuad(-0.25f,0,1.025f,
                 0.25f,0,1.025f,
                 0.25f,1.2f,1.025f,
                -0.25f,1.2f,1.025f);
    }
}

House::House() {
    wallsStart = vertexCount();
    addWalls();
    wallsCount = vertexCount() - wallsStart;

    roofStart = vertexCount();
    addRoof();
    roofCount = vertexCount() - roofStart;

    doorStart = vertexCount();
    addDoor();
    doorCount = vertexCount() - doorStart;

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

