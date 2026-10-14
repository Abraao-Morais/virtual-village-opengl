#include "rendering/shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

std::string readFile(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open())
        throw std::runtime_error("Nao foi possivel abrir o shader: " + path);

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

GLuint compileShader(GLenum type, const std::string& source, const std::string& path) {
    GLuint shader = glCreateShader(type);

    const char* sourcePointer = source.c_str();

    glShaderSource(shader, 1, &sourcePointer, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLint logLength = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::string infoLog(static_cast<std::size_t>(logLength), '\0');

        glGetShaderInfoLog(shader, logLength, nullptr, &infoLog[0]);

        std::cerr << "Erro ao compilar shader " << path << ":\n" << infoLog << '\n';

        glDeleteShader(shader);

        throw std::runtime_error("Falha na compilacao do shader: " + path);
    }

    return shader;
}

}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) : programID(0) {
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;

    try {
        const std::string vertexSource = readFile(vertexPath);
        const std::string fragmentSource = readFile(fragmentPath);

        vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);
        fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);
        programID = glCreateProgram();

        glAttachShader(programID, vertexShader);
        glAttachShader(programID, fragmentShader);

        glLinkProgram(programID);
        GLint success = GL_FALSE;

        glGetProgramiv(programID, GL_LINK_STATUS, &success);

        if (!success) {
            GLint logLength = 0;

            glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);

            std::string infoLog(static_cast<std::size_t>(logLength), '\0');

            glGetProgramInfoLog(programID, logLength, nullptr, &infoLog[0]);

            std::cerr << "Erro ao vincular programa de shaders:\n" << infoLog << '\n';

            throw std::runtime_error("Falha ao vincular programa de shaders.");
        }

        glDetachShader(programID, vertexShader);
        glDetachShader(programID, fragmentShader);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    } catch (...) {
        if (programID != 0) {
            glDeleteProgram(programID);
            programID = 0;
        }

        if (vertexShader != 0)
            glDeleteShader(vertexShader);

        if (fragmentShader != 0)
            glDeleteShader(fragmentShader);

        throw;
    }
}

Shader::~Shader() {
    if (programID != 0) glDeleteProgram(programID);
}

void Shader::use() const {
    glUseProgram(programID);
}

GLuint Shader::id() const {
    return programID;
}