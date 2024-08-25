#ifndef RENDERER_H
#define RENDERER_H

#include "Window.h"
#include "Model.h"
#include "Camera.h"
#include "shader.hpp"

class Renderer {
public:
    Renderer(Window& window, Camera& camera);
    ~Renderer();

    bool init();
    void render(Model& model);
    void cleanup();
    void setLightPosition(const glm::vec3& position);
    Window& getWindow();

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
};

#endif
