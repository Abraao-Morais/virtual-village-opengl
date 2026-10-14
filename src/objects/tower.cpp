#include "objects/tower.h"

#include "objects/mesh_utils.h"
#include <vector>
#include <cmath>

using MeshUtils::addTriangle;
using MeshUtils::addQuad;
using MeshUtils::vertexCount;

namespace {
    constexpr float PI = 3.14159265358979323846f;

    constexpr float TOWER_RADIUS = 2.5f;
    constexpr float TOWER_HEIGHT = 12.0f;
    constexpr float BASE_RADIUS = 2.65f;
    constexpr float BASE_HEIGHT = 0.35f;

    constexpr int SEGMENTS = 32;

    std::vector<float> vertices;

    float angleAt(int segment) {
        return 2.0f * PI * segment / SEGMENTS;
    }

    void addCylinderWall() {
        for (int i = 0; i < SEGMENTS; ++i) {
            const float a0 = angleAt(i);
            const float a1 = angleAt(i + 1);

            const float x0 = TOWER_RADIUS * std::sin(a0);
            const float z0 = TOWER_RADIUS * std::cos(a0);

            const float x1 = TOWER_RADIUS * std::sin(a1);
            const float z1 = TOWER_RADIUS * std::cos(a1);

            addQuad(vertices,
                x0, 0.0f, z0,
                x1, 0.0f, z1,
                x1, TOWER_HEIGHT, z1,
                x0, TOWER_HEIGHT, z0
            );
        }
    }

    void addTop() {
        const float radius = BASE_RADIUS;
        const float y = TOWER_HEIGHT;

        for (int i = 0; i < SEGMENTS; ++i) {
            const float a0 = angleAt(i);
            const float a1 = angleAt(i + 1);

            const float x0 = radius * std::sin(a0);
            const float z0 = radius * std::cos(a0);

            const float x1 = radius * std::sin(a1);
            const float z1 = radius * std::cos(a1);

            addTriangle(vertices,
                0.0f, y, 0.0f,
                x0, y, z0,
                x1, y, z1
            );
        }
    }

    void addBase() {
        const float innerRadius = TOWER_RADIUS - 0.08f;

        for (int i = 0; i < SEGMENTS; ++i) {
            const float a0 = angleAt(i);
            const float a1 = angleAt(i + 1);

            const float x0 = BASE_RADIUS * std::sin(a0);
            const float z0 = BASE_RADIUS * std::cos(a0);
            const float x1 = BASE_RADIUS * std::sin(a1);
            const float z1 = BASE_RADIUS * std::cos(a1);

            const float ix0 = innerRadius * std::sin(a0);
            const float iz0 = innerRadius * std::cos(a0);
            const float ix1 = innerRadius * std::sin(a1);
            const float iz1 = innerRadius * std::cos(a1);

            // Parte externa da base.
            addQuad(vertices, x0, 0.0f, z0, x1, 0.0f, z1, x1, BASE_HEIGHT, z1, x0, BASE_HEIGHT, z0);

            // Superfície superior da base.
            addQuad(vertices, x0, BASE_HEIGHT, z0, x1, BASE_HEIGHT, z1, ix1, BASE_HEIGHT, iz1, ix0, BASE_HEIGHT, iz0);

            // Superfície inferior da base.
            addQuad(vertices, ix0, 0.0f, iz0, ix1, 0.0f, iz1, x1, 0.0f, z1, x0, 0.0f, z0);
        }
    }

    void addBox(float x0, float y0, float z0, float x1, float y1, float z1) {
        // Frente.
        addQuad(vertices, x0, y0, z1, x1, y0, z1, x1, y1, z1, x0, y1, z1);

        // Trás.
        addQuad(vertices, x1, y0, z0, x0, y0, z0,x0, y1, z0, x1, y1, z0);

        // Lateral esquerdo.
        addQuad(vertices, x0, y0, z0, x0, y0, z1, x0, y1, z1, x0, y1, z0);

        // Lateral direito.
        addQuad(vertices, x1, y0, z1, x1, y0, z0, x1, y1, z0, x1, y1, z1);

        // Topo.
        addQuad(vertices, x0, y1, z1, x1, y1, z1, x1, y1, z0, x0, y1, z0);

        // Base.
        addQuad(vertices, x0, y0, z0, x1, y0, z0, x1, y0, z1, x0, y0, z1);
    }

    void addBattlements() {
        const int count = 16;
        const float blockWidth = 0.50f;
        const float blockHeight = 0.80f;
        const float blockDepth = 0.50f;
        const float radius = 2.48f;

        for (int i = 0; i < count; ++i) {
            const float angle = 2.0f * PI * i / count;

            const float x = radius * std::sin(angle);
            const float z = radius * std::cos(angle);

            // Blocos distribuídos pelo perímetro.
            // A rotação individual é aproximada pela posição.
            addBox(
                x - blockWidth * 0.5f,
                TOWER_HEIGHT,
                z - blockDepth * 0.5f,
                x + blockWidth * 0.5f,
                TOWER_HEIGHT + blockHeight,
                z + blockDepth * 0.5f
            );
        }
    }
}

Tower::Tower() {
    // Corpo cilíndrico.
    wallsStart = vertexCount(vertices);
    addCylinderWall();
    wallsCount = vertexCount(vertices) - wallsStart;

    // Base reforçada.
    baseStart = vertexCount(vertices);
    addBase();
    baseCount = vertexCount(vertices) - baseStart;

    topStart = vertexCount(vertices);
    addTop();
    topCount = vertexCount(vertices) - topStart;

    // Ameias.
    battlementsStart = vertexCount(vertices);
    addBattlements();
    battlementsCount = vertexCount(vertices) - battlementsStart;

    // Upload da geometria para a GPU.
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

Tower::~Tower() {
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);

    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
}

void Tower::draw() const {
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, wallsStart, wallsCount);
    glDrawArrays(GL_TRIANGLES, baseStart, baseCount);
    glDrawArrays(GL_TRIANGLES, topStart, topCount);
    glDrawArrays(GL_TRIANGLES, battlementsStart, battlementsCount);

    glBindVertexArray(0);
}

void Tower::draw(GLint colorLocation) const {
    glBindVertexArray(VAO);

    // Pedra principal.
    glUniform3f(colorLocation, 0.48f, 0.46f, 0.42f);
    glDrawArrays(GL_TRIANGLES, wallsStart, wallsCount);

    // Pedra mais escura na base.
    glUniform3f(colorLocation, 0.36f, 0.34f, 0.31f);
    glDrawArrays(GL_TRIANGLES, baseStart, baseCount);

    // Tampa superior.
    glUniform3f(colorLocation, 0.52f, 0.49f, 0.44f);
    glDrawArrays(GL_TRIANGLES, topStart, topCount);

    // Ameias.
    glUniform3f(colorLocation, 0.55f, 0.52f, 0.47f);
    glDrawArrays(GL_TRIANGLES, battlementsStart, battlementsCount);

    glBindVertexArray(0);
}