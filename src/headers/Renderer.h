#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "Window.h"
#include "Model.h"
#include "Camera.h"
#include "shader.hpp"
#include "Lights.h"
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
    void renderScene(Model& model);

    // Set methods for light and ambient intensity
    void setAmbientLightIntensity(const glm::vec3& intensity);

    // Render function to apply all lights
    void renderLightsForObject();

    // Get methods
    glm::vec3 getAmbientLightIntensity() const;
    Camera& getCamera();
    Window& getWindow();

    // Getter and setter for lights
    std::vector<Lights>& getPointLights();
    std::vector<Lights>& getDirectionalLights();
    std::vector<Lights>& getSpotLights();

    void addPointLight(const Lights& light);
    void addDirectionalLight(const Lights& light);
    void addSpotLight(const Lights& light);

    // Cleanup resources
    void cleanup();

private:
    // Reference to window and camera objects
    Window& window;
    Camera& camera;

    // Shader program IDs
    GLuint programShaderID;// Initialize OpenGL, shaders, and GLEW for obj(model) 
    GLuint infiniteGroundShaderID;

    // Uniform locations for object shader
    GLuint MatrixID;
    GLuint ViewMatrixID;
    GLuint ModelMatrixID;
    GLuint LightID;
    GLuint AmbientLightID;

    // Projection matrix for the camera
    glm::mat4 Projection;

    // Light properties
    glm::vec3 ambientLightIntensity;

    //Lights
    std::vector<Lights> pointLights;        // Store multiple point lights
    std::vector<Lights> directionalLights;  // Store multiple directional lights
    std::vector<Lights> spotLights;         // Store multiple spot lights

    // VAO and VBO for object rendering
    GLuint vao;
    GLuint vbo;

    // Infinite ground plane
    std::unique_ptr<InfiniteGround> infiniteGround;

    // Helper flags
    bool positionPrinted;      // To print position once
    bool groundHeightSet;      // Track whether the ground height is set

    void setupMaterialsForObject(Model& model);

    //Default Scene Lights Setup
    void setupPointLight();
    void setupDirectionalLight();
    void setupSpotLight();
};

#endif // RENDERER_H
