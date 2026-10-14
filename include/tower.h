#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Tower {
    public:
        Tower();
        ~Tower();

        void draw() const;
        void draw(GLint colorLocation) const;

    private:
        GLuint VAO = 0;
        GLuint VBO = 0;

        // Corpo circular de pedra
        GLint wallsStart = 0;
        GLint wallsCount = 0;

        // Base inferior reforçada
        GLint baseStart = 0;
        GLint baseCount = 0;

        GLint topStart = 0;
        GLint topCount = 0;

        // Ameias no topo da torre
        GLint battlementsStart = 0;
        GLint battlementsCount = 0;
};