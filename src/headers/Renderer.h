#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "Window.h"
#include "Model.h"
#include "Camera.h"
#include "shader.hpp"
#include "InfiniteGround.h"
#include <glm/glm.hpp>
#include <vector>
#include <imgui.h>

class Renderer {
public:
    // Constructor and Destructor
    Renderer(Window& window, Camera& camera);
    ~Renderer();

    // Initialize OpenGL settings and load shaders
    bool init();

    // Render a model
    void render(Model& model);

    // Set methods for light and ambient intensity
    void setAmbientLightIntensity(const glm::vec3& intensity);
    void setLightPosition(const glm::vec3& position);

    // Get methods
    glm::vec3 getAmbientLightIntensity() const;
    Camera& getCamera();
    Window& getWindow();

    // Cleanup resources
    void cleanup();

private:
    // Reference to window and camera objects
    Window& window;
    Camera& camera;

    // Shader program IDs
    GLuint programID;
    GLuint infiniteGroundShader;

    // Uniform locations for object shader
    GLuint MatrixID;
    GLuint ViewMatrixID;
    GLuint ModelMatrixID;
    GLuint LightID;
    GLuint AmbientLightID;

    // Projection matrix for the camera
    glm::mat4 Projection;

    // Light properties
    glm::vec3 lightPos;
    glm::vec3 lightIntensity;
    glm::vec3 ambientLightIntensity;

    // Directional light control variables
    glm::vec3 dirLightColor;
    glm::vec3 dirLightDirection;
    glm::vec3 dirLightIntensity;

    // VAO and VBO for object rendering
    GLuint vao;
    GLuint vbo;

    // Infinite ground plane
    std::unique_ptr<InfiniteGround> infiniteGround;

    // Helper flags
    bool positionPrinted;      // To print position once
    bool groundHeightSet;      // Track whether the ground height is set
};

#endif // RENDERER_H
