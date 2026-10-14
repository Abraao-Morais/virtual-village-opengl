#include "objects/gravestone.h"

#include <vector>
#include <cmath>
#include <array>
#include <glm/glm.hpp>

#include "objects/mesh_utils.h"

using MeshUtils::addTriangle;
using MeshUtils::addQuad;
using MeshUtils::vertexCount;

namespace {
    constexpr float PI = 3.14159265358979323846f;
    constexpr int ARC_SEGMENTS = 12;
    std::vector<float> vertices;

    void addBox(float x0, float y0, float z0, float x1, float y1, float z1) {
        // Frente (+Z).
        addQuad(vertices, x0,y0,z1, x1,y0,z1, x1,y1,z1, x0,y1,z1);
        // Fundo (-Z).
        addQuad(vertices, x1,y0,z0, x0,y0,z0, x0,y1,z0, x1,y1,z0);
        // Laterais.
        addQuad(vertices, x0,y0,z0, x0,y0,z1, x0,y1,z1, x0,y1,z0);
        addQuad(vertices, x1,y0,z1, x1,y0,z0, x1,y1,z0, x1,y1,z1);
        // Topo e base.
        addQuad(vertices, x0,y1,z1, x1,y1,z1, x1,y1,z0, x0,y1,z0);
        addQuad(vertices, x0,y0,z0, x1,y0,z0, x1,y0,z1, x0,y0,z1);
    }

    // Placa com topo semicircular, extrudida para dar volume.
    void addStoneShape() {
        constexpr float halfWidth = 0.52f;
        constexpr float bottomY = 0.30f;
        constexpr float arcCenterY = 1.28f;
        constexpr float depth = 0.16f;

        // Contorno visto de frente: lados retos e arco superior.
        std::vector<glm::vec2> outline;
        outline.emplace_back(-halfWidth, bottomY);
        outline.emplace_back( halfWidth, bottomY);
        outline.emplace_back( halfWidth, arcCenterY);
        for (int i = 1; i <= ARC_SEGMENTS; ++i) {
            const float angle = PI * static_cast<float>(i) / ARC_SEGMENTS;
            outline.emplace_back(halfWidth * std::cos(angle), arcCenterY + halfWidth * std::sin(angle));
        }

        // Frente e verso triangulados a partir de um ponto interno.
        const glm::vec2 center(0.0f, 0.90f);
        for (std::size_t i = 0; i < outline.size(); ++i) {
            const glm::vec2 a = outline[i];
            const glm::vec2 b = outline[(i + 1) % outline.size()];

            addTriangle(vertices,
                center.x, center.y, depth,
                a.x, a.y, depth,
                b.x, b.y, depth);
            addTriangle(vertices,
                center.x, center.y, -depth,
                b.x, b.y, -depth,
                a.x, a.y, -depth);

            // Parede lateral entre os pontos consecutivos do contorno.
            addQuad(vertices,
                a.x, a.y, depth,
                a.x, a.y, -depth,
                b.x, b.y, -depth,
                b.x, b.y, depth);
        }
    }

    void addFrontCross() {
        // Cruz discreta em pedra clara na face frontal (+Z).
        constexpr float z = 0.166f;
        // Haste vertical.
        addQuad(vertices,
            -0.045f, 0.68f, z,
             0.045f, 0.68f, z,
             0.045f, 1.42f, z,
            -0.045f, 1.42f, z);
        // Braços.
        addQuad(vertices,
            -0.22f, 1.16f, z,
             0.22f, 1.16f, z,
             0.22f, 1.25f, z,
            -0.22f, 1.25f, z);
    }
}

Gravestone::Gravestone() {
    baseStart = vertexCount(vertices);
    addBox(-0.72f, 0.0f, -0.32f, 0.72f, 0.30f, 0.32f);
    baseCount = vertexCount(vertices) - baseStart;

    stoneStart = vertexCount(vertices);
    addStoneShape();
    stoneCount = vertexCount(vertices) - stoneStart;

    detailStart = vertexCount(vertices);
    addFrontCross();
    detailCount = vertexCount(vertices) - detailStart;

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

Gravestone::~Gravestone() {
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
}

void Gravestone::draw() const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, baseStart, baseCount);
    glDrawArrays(GL_TRIANGLES, stoneStart, stoneCount);
    glDrawArrays(GL_TRIANGLES, detailStart, detailCount);
    glBindVertexArray(0);
}

void Gravestone::draw(GLint colorLocation) const {
    glBindVertexArray(VAO);

    // Base um pouco mais escura para separar visualmente do corpo.
    glUniform3f(colorLocation, 0.30f, 0.29f, 0.28f);
    glDrawArrays(GL_TRIANGLES, baseStart, baseCount);

    // Pedra cinza fria.
    glUniform3f(colorLocation, 0.48f, 0.48f, 0.46f);
    glDrawArrays(GL_TRIANGLES, stoneStart, stoneCount);

    // Cruz em pedra clara.
    glUniform3f(colorLocation, 0.72f, 0.70f, 0.65f);
    glDrawArrays(GL_TRIANGLES, detailStart, detailCount);

    glBindVertexArray(0);
}
