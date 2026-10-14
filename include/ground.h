#pragma once

#include <glad/glad.h>

class Ground {
    public:
        Ground();
        ~Ground();

        void draw();

        Ground(const Ground&) = delete;
        Ground& operator=(const Ground&) = delete;

    private:
        GLuint VAO = 0;
        GLuint VBO = 0;
};