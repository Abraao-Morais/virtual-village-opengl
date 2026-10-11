#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera();

    void processKeyboard(
        int key,
        float deltaTime
    );

    void processMouse(
        double xpos,
        double ypos
    );

    glm::mat4 getViewMatrix() const;

    glm::vec3 getPosition() const;

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;

    bool firstMouse;
    double lastX;
    double lastY;

    void updateCameraVectors();
};