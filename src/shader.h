#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() const;

    GLuint id() const;

private:
    GLuint programID;
};

#endif