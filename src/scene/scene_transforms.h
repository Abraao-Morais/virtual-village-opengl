#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SceneTransforms {

    inline glm::mat4 createModel(const glm::vec3& position, float rotationYDegrees = 0.0f, const glm::vec3& scale = glm::vec3(1.0f)) {
        glm::mat4 model(1.0f);

        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotationYDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, scale);

        return model;
    }
}