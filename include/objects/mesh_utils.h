#pragma once

#include <array>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace MeshUtils {

    constexpr int SIDES = 8;
    constexpr float PI = 3.14159265358979323846f;

    inline GLint vertexCount(const std::vector<float>& vertices) {
        return static_cast<GLint>(vertices.size() / 3);
    }

    inline void addTriangle(std::vector<float>& vertices, float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz) {
        vertices.insert(vertices.end(), {ax, ay, az, bx, by, bz, cx, cy, cz});
    }

    inline void addQuad(std::vector<float>& vertices, float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz, float dx, float dy, float dz) {
        addTriangle(vertices, ax, ay, az, bx, by, bz, cx, cy, cz);
        addTriangle(vertices, ax, ay, az,cx, cy, cz, dx, dy, dz);
    }

    inline std::array<glm::vec2, SIDES> calculateOctagon(float centerX,float centerZ, float radius, float startAngle = 0.0f) {
        std::array<glm::vec2, SIDES> points;

        for (int i = 0; i < SIDES; ++i) {
            float angle = startAngle + (2.0f * PI * i / SIDES);
            points[i] = glm::vec2(centerX + radius * std::cos(angle), centerZ + radius * std::sin(angle));
        }

        return points;
    }

}