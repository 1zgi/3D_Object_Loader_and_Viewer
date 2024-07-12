#include"Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
    Camera::width = width;
    Camera::height = height;
    Position = position;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
    // Initializes matrices since otherwise they will be the null matrix
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // Makes camera look in the right direction from the right position
    view = glm::lookAt(Position, Position + Orientation, Up);
    // Adds perspective to the scene
    projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

    // Sets new camera matrix
    cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
    // Exports camera matrix
     // Exports the camera matrix to the Vertex Shader
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}


void Camera::Inputs(SDL_Window * window)
{
        SDL_Event e;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                // Handle quit event.
            }
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);

        // Handle keyboard input
        if (state[SDL_SCANCODE_W]) {
            Position += speed * Orientation;
        }
        if (state[SDL_SCANCODE_A]) {
            Position += speed * -glm::normalize(glm::cross(Orientation, Up));
        }
        if (state[SDL_SCANCODE_S]) {
            Position += speed * -Orientation;
        }
        if (state[SDL_SCANCODE_D]) {
            Position += speed * glm::normalize(glm::cross(Orientation, Up));
        }
        if (state[SDL_SCANCODE_SPACE]) {
            Position += speed * Up;
        }
        if (state[SDL_SCANCODE_LCTRL]) {
            Position += speed * -Up;
        }
        if (state[SDL_SCANCODE_LSHIFT]) {
            speed = 0.4f;
        }
        else 
        {
            speed = 0.1f;
        }

        // Handle mouse input
        int mouseX, mouseY;
        Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) 
        {
            if (firstClick) {
                SDL_WarpMouseInWindow(window, width / 2, height / 2);
                firstClick = false;
            }

            float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
            float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

            glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));
            if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f)) {
                Orientation = newOrientation;
            }

            Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

            SDL_WarpMouseInWindow(window, width / 2, height / 2);
            SDL_ShowCursor(SDL_DISABLE);
        }
        else {
            firstClick = true;
            SDL_ShowCursor(SDL_ENABLE);
        }
}