#include "objects/windmill.h"

#include "objects/mesh_utils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <cmath>
#include <vector>

using MeshUtils::addTriangle;
using MeshUtils::addQuad;
using MeshUtils::calculateOctagon;
using MeshUtils::vertexCount;

namespace {
    std::vector<float> vertices;
    constexpr int SIDES = 8;

    void addWalls(float height, float radius) {
        auto points = calculateOctagon(0.0f, 0.0f, radius);
        auto superiorPoints = calculateOctagon(0.0f, 0.0f, radius * 0.6f);

        for (int i = 0; i < SIDES; ++i) {
            int next = (i + 1) % SIDES;

            float x1 = points[i].x;
            float z1 = points[i].y;
            float superiorx1 = superiorPoints[i].x;
            float superiorz1 = superiorPoints[i].y;

            float x2 = points[next].x;
            float z2 = points[next].y;
            float superiorx2 = superiorPoints[next].x;
            float superiorz2 = superiorPoints[next].y;

            addQuad(vertices,
                x1, 0.0f, z1,
                x2, 0.0f, z2,
                superiorx2, height, superiorz2,
                superiorx1, height, superiorz1
            );
        }
    }

    void addRoof(float height, float radius) {
        auto points = calculateOctagon(0.0f, 0.0f, radius * 0.7f);

        for (int i = 0; i < SIDES; ++i) {
            int next = (i + 1) % SIDES;

            float x1 = points[i].x;
            float z1 = points[i].y;

            float x2 = points[next].x;
            float z2 = points[next].y;

            addTriangle(vertices,
                x1, height - 0.3f, z1,
                x2, height - 0.3f, z2,
                0.0f, height + 2.5f, 0.0f
            );
        }
    }

    void addBlade(float angle) {
        const float innerRadius = 0.25f;
        const float outerRadius = 3.3f;
        const float halfWidth = 0.35f;

        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

        auto transform = [&](float x, float y) { 
            glm::vec4 p = rotation * glm::vec4(x, y, 0.0f, 1.0f);
            return glm::vec3(p);
        };

        glm::vec3 a = transform(-halfWidth, innerRadius);
        glm::vec3 b = transform( halfWidth, innerRadius);
        glm::vec3 c = transform( halfWidth, outerRadius);
        glm::vec3 d = transform(-halfWidth, outerRadius);

        addQuad(vertices,
            a.x, a.y, a.z,
            b.x, b.y, b.z,
            c.x, c.y, c.z,
            d.x, d.y, d.z
        );
    }

    void addBlades() {
        for (int i = 0; i < 4; ++i)
            addBlade(glm::radians(90.0f * i));
    }
}

Windmill::Windmill() {
    wallsStart = vertexCount(vertices);
    addWalls(9.0f, 3.0f);
    wallsCount = vertexCount(vertices) - wallsStart;

    roofStart = vertexCount(vertices);
    addRoof(9.0f, 3.0f);
    roofCount = vertexCount(vertices) - roofStart;

    bladesStart = vertexCount(vertices);
    addBlades();
    bladesCount = vertexCount(vertices) - bladesStart;

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

Windmill::~Windmill() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Windmill::draw() const {
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, wallsStart, wallsCount);
    glDrawArrays(GL_TRIANGLES, roofStart, roofCount);

    glBindVertexArray(0);
}

void Windmill::draw(GLint colorLocation) const {
    glBindVertexArray(VAO);

    glUniform3f(colorLocation, 0.5255f, 0.4784f, 0.4275f);
    glDrawArrays(GL_TRIANGLES, wallsStart, wallsCount);

    glUniform3f(colorLocation, 0.2588f, 0.2431f, 0.2823f);
    glDrawArrays(GL_TRIANGLES, roofStart, roofCount);

    glBindVertexArray(0);
}

void Windmill::drawBlades() const {
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, bladesStart, bladesCount);
    glBindVertexArray(0);
}

void Windmill::drawBlades(GLint colorLocation) const {
    glBindVertexArray(VAO);

    glUniform3f(colorLocation, 0.2078f, 0.2f, 0.2274f);
    glDrawArrays(GL_TRIANGLES, bladesStart, bladesCount);
    glBindVertexArray(0);
}

