#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
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
    
    //get methods
    glm::vec3 getAmbientLightIntensity() const;
    Camera& getCamera();
    Window& getWindow();

    //set methods
    void setAmbientLightIntensity(const glm::vec3& intensity);
    void setLightPosition(const glm::vec3& position);
    

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
    glm::vec3 dirLightDirection;
    glm::vec3 dirLightIntensity;

};

#endif // RENDERER_H
