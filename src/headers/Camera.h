#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);

    glm::mat4 getViewMatrix() const;
    void handleMouseMotion(int xrel, int yrel);
    void handleMouseScroll(float yOffset);
    void handleMouseButton(bool mousePressed);

    glm::vec3 getPosition() const;
    bool isMouseHeld() const;  // Add this function to check if the mouse is held

private:
    void updateCameraVectors();

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    glm::vec3 target;

    float yaw;
    float pitch;

    float distanceFromTarget;
    bool mouseHeld;  // This boolean tracks whether the mouse button is pressed
};

#endif
