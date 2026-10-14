#ifndef PUMPKIN_H
#define PUMPKIN_H

#include <glad/glad.h>
#include <glm/glm.hpp>

class Pumpkin {
    public:
        Pumpkin();
        ~Pumpkin();

        void draw() const;
        void draw(GLint colorLocation) const;

    private:
        GLuint VAO, VBO;
        GLsizei bodyStart, bodyCount;
        GLsizei stemStart, stemCount;
        GLsizei eyesStart, eyesCount;
        GLsizei mouthStart, mouthCount;
};

#endif