#pragma once

#include <glad/glad.h>

// Igreja gótica construída com triângulos e quadriláteros, seguindo
// a mesma abordagem procedural usada por House e Windmill.
class Church {
public:
    Church();
    ~Church();

    Church(const Church&) = delete;
    Church& operator=(const Church&) = delete;

    void draw() const;
    void draw(GLint colorLocation) const;

private:
    GLuint VAO = 0;
    GLuint VBO = 0;

    GLint stoneStart = 0, stoneCount = 0;
    GLint roofStart = 0, roofCount = 0;
    GLint doorStart = 0, doorCount = 0;
    GLint windowStart = 0, windowCount = 0;
    GLint trimStart = 0, trimCount = 0;
};
