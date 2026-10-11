#pragma once

#include <glad/glad.h>

class Ground {
public:
    Ground();
    ~Ground();

    void draw();

    // Impede cópias acidentais dos buffers da GPU.
    Ground(const Ground&) = delete;
    Ground& operator=(const Ground&) = delete;

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
};