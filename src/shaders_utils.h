#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ShaderUtils {

    inline void setMat4(GLint location, const glm::mat4& matrix) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    inline void setColor(GLint location, const glm::vec3& color) {
        glUniform3fv(location, 1, glm::value_ptr(color));
    }

    inline void setFloat(GLint location, float value) {
        glUniform1f(location, value);
    }

    inline void setInt(GLint location, int value) {
        glUniform1i(location, value);
    }
}
