#ifndef RENDERER_H
#define RENDERER_H

#include "Window.h"
#include "Model.h"
#include "Camera.h"
#include "shader.hpp"
#include <glm/glm.hpp>
#include <imgui.h>

class Renderer {
public:
    Renderer(Window& window, Camera& camera);
    ~Renderer();

    bool init();
    void render(Model& model);
    void cleanup();
    void setLightPosition(const glm::vec3& position);
    Camera& getCamera();
    Window& getWindow();

    glm::vec3 dirLightDirection = glm::vec3(-1.0f, -1.0f, -1.0f);
    glm::vec3 dirLightIntensity = glm::vec3(1.0f, 1.0f, 1.0f);

private:

    Window& window;
    Camera& camera;

    GLuint programID;
    GLuint MatrixID;
    GLuint ViewMatrixID;
    GLuint ModelMatrixID;
    GLuint LightID;
    GLuint AmbientLightID;

    glm::mat4 Projection;
    glm::vec3 lightPos;
    glm::vec3 lightIntensity;
    glm::vec3 ambientLightIntensity;

    GLuint vao;
    GLuint vbo;

    bool positionPrinted;

    // Directional light control variables
    glm::vec3 dirLightColor;
    glm::vec3 pointLightPosition;
    glm::vec3 pointLightColor;
    float pointLightIntensity;
};

#endif // RENDERER_H
