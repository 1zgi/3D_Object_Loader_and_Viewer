#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "Window.h"
#include "Model.h"
#include "Camera.h"
#include "shader.hpp"
#include "InfiniteGround.h"
#include <glm/glm.hpp>
#include <imgui.h>

class Renderer {
public:
    Renderer(Window& window, Camera& camera);
    ~Renderer();

    bool init();
    void render(Model& model);
    void addObject(std::shared_ptr<Model> obj);
   
    //set methods
    void setAmbientLightIntensity(const glm::vec3& intensity);
    void setLightPosition(const glm::vec3& position);
    glm::vec3 getAmbientLightIntensity() const;

    //get methods
    Camera& getCamera();
    Window& getWindow();

    void cleanup();

private:

    Window& window;
    Camera& camera;

    GLuint programID;
    GLuint infiniteGroundShader;

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

    std::unique_ptr<InfiniteGround> infiniteGround;
    std::vector<std::shared_ptr<Model>> objects;  // To store multiple objects to render
};

#endif // RENDERER_H
