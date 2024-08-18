#include "headers/Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
    : position(position), worldUp(up), yaw(-38.0f), pitch(-30.0f) {
    front = glm::normalize(target - position);
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::handleKeyboardInput(const Uint8* state, float deltaTime) {
    float cameraSpeed = 2.5f * deltaTime;
    if (state[SDL_SCANCODE_W]) {
        position += cameraSpeed * front;
    }
    if (state[SDL_SCANCODE_S]) {
        position -= cameraSpeed * front;
    }
    if (state[SDL_SCANCODE_A]) {
        position -= cameraSpeed * right;
    }
    if (state[SDL_SCANCODE_D]) {
        position += cameraSpeed * right;
    }
}

void Camera::handleMouseMotion(int xrel, int yrel) {
    float sensitivity = 0.1f;
    yaw += xrel * sensitivity;
    pitch -= yrel * sensitivity;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front = glm::normalize(front);
    right = glm::normalize(glm::cross(this->front, worldUp));
    up = glm::normalize(glm::cross(right, this->front));
}
