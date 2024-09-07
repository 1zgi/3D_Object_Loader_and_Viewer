#include "headers/Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
    : position(position), target(target), worldUp(up), yaw(-25.0f), pitch(-25.0f), distanceFromTarget(5.0f), mouseHeld(false) {
    front = glm::normalize(target - position);
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, target, up);  // Keep focus on the target
}

void Camera::handleMouseMotion(int xrel, int yrel) {
    if (mouseHeld) {
        float sensitivity = 0.2f;
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
}

void Camera::handleMouseButton(bool mousePressed) {
    mouseHeld = mousePressed;  // Update mouseHeld when the button is pressed or released
}

bool Camera::isMouseHeld() const {
    return mouseHeld;  // Return whether the mouse button is being held
}

void Camera::handleMouseScroll(float yOffset) {
    distanceFromTarget -= yOffset * 0.5f;
    if (distanceFromTarget < 1.0f) {
        distanceFromTarget = 1.0f;
    }
    if (distanceFromTarget > 20.0f) {
        distanceFromTarget = 20.0f;
    }
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    this->front = glm::normalize(front);

    position = target - front * distanceFromTarget;

    right = glm::normalize(glm::cross(worldUp, this->front));
    up = glm::normalize(glm::cross(this->front, right));
}

glm::vec3 Camera::getPosition() const {
    return position;
}
