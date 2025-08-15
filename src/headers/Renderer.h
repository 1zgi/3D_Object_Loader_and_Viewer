#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "Window.h"
#include "Model.h"
#include "Camera.h"
#include "shader.hpp"
#include "Lights.h"
#include "ShadowMap.h"
#include "InfiniteGround.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <imgui.h>

class Renderer {
public:
    // Constructor and Destructor
    Renderer(Window& window, Camera& camera, Model& model);
    ~Renderer();

    // Initialize OpenGL settings and load shaders
    bool init();

    // Render a model
    void renderScene();

    // Set methods for light and ambient intensity
    void setAmbientLightIntensity(const glm::vec3& intensity);

    // Auto rotation methods
    void setAutoRotation(bool enabled);
    bool getAutoRotation() const;
    void setRotationSpeed(float speed);

    // Shadow control methods
    void setShadowsEnabled(bool enabled);
    bool getShadowsEnabled() const;

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

    void setModel(const Model& model);

    // Cleanup resources
    void cleanup();

private:
    // Reference to window and camera objects
    Window& window;
    Camera& camera;
    ShadowMap shadowMap;
    Model& model;

    // Shader program IDs
    GLuint programShaderID;// Initialize OpenGL, shaders, and GLEW for obj(model) 
    GLuint infiniteGroundShaderID;
    GLuint shadowMapShaderID;

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

    glm::vec3 lightPos;
    glm::vec3 targetPos;

    // Infinite ground plane
    std::unique_ptr<InfiniteGround> infiniteGround;

    // Helper flags
    bool positionPrinted;      // To print position once
    bool groundHeightSet;      // Track whether the ground height is set
    bool autoRotateModel;      // Flag to enable/disable auto rotation
    float rotationSpeed;       // Speed of rotation in degrees per second
    bool shadowsEnabled;       // Flag to enable/disable shadows

    void MatrixUniformLocations(GLuint programShaderID);
    void MatrixPassToShader(const glm::mat4& MVP, const glm::mat4& View, const glm::mat4& Model);
    void renderToTheDepthTexture();
    glm::mat4 calculateMVP(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    void setShadowMatrixUniform(GLuint shaderID, const glm::mat4& shadowMatrix);
    void renderGroundWithShadows(const glm::mat4& View, const glm::mat4& Projection);
    void renderModelWithShadows(const glm::mat4& View, const glm::mat4& Projection);
    void bindShadowMap(GLuint shaderID, GLuint textureUnit);

    //Default Scene Lights Setup
    void setupPointLight();
    void setupDirectionalLight();
    void setupSpotLight();
};

#endif // RENDERER_H
