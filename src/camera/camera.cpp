#include "camera/camera.h"

#include <GLFW/glfw3.h>
#include <cmath>

Camera::Camera()
    : position(0.0f, 1.7f, 5.0f),
      front(0.0f, 0.0f, -1.0f),
      up(0.0f, 1.0f, 0.0f),
      worldUp(0.0f, 1.0f, 0.0f),
      yaw(-90.0f),
      pitch(0.0f),
      movementSpeed(5.0f),
      mouseSensitivity(0.1f),
      firstMouse(true),
      lastX(400.0),
      lastY(300.0) {
    updateCameraVectors();
}

void Camera::processKeyboard(int key, float deltaTime) {
    float velocity = movementSpeed * deltaTime;

    if (key == GLFW_KEY_W) position += front * velocity;
    if (key == GLFW_KEY_S) position -= front * velocity;
    if (key == GLFW_KEY_A) position -= right * velocity;
    if (key == GLFW_KEY_D) position += right * velocity;

    // Mantém a câmera na altura dos olhos.
    position.y = 1.7f;
}

void Camera::processMouse(double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xOffset = static_cast<float>(xpos - lastX);
    float yOffset = static_cast<float>(lastY - ypos);

    lastX = xpos;
    lastY = ypos;

    yaw += xOffset * mouseSensitivity;
    pitch += yOffset * mouseSensitivity;

    // Evita que a câmera vire de cabeça para baixo.
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 direction;

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(direction);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getPosition() const {
    return position;
}