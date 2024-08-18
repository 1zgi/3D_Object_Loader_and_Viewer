#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);

    glm::mat4 getViewMatrix() const;
    void handleKeyboardInput(const Uint8* state, float deltaTime);
    void handleMouseMotion(int xrel, int yrel);

private:
    void updateCameraVectors();

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
};

#endif
