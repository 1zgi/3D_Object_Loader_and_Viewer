#include "headers/Renderer.h"

Renderer::Renderer(Window& window, Camera& camera)
    : window(window),
    camera(camera),
    programShaderID(0),
    infiniteGroundShaderID(0),
    MatrixID(0),
    ViewMatrixID(0),
    ModelMatrixID(0),
    LightID(0),
    AmbientLightID(0),
    Projection(glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
    ambientLightIntensity(0.2f, 0.2f, 0.2f),
    vao(0),
    vbo(0),
    infiniteGround(std::make_unique<InfiniteGround>()),
    positionPrinted(false),
    groundHeightSet(false){}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::init() {
    // Initialize OpenGL, shaders, and GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return false;
    }

    // Set the background (clear) color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Enable depth test and culling
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Load the shader programs
    programShaderID = LoadShaders("src/shaders/vert.glsl", "src/shaders/frag.glsl");
    infiniteGroundShaderID = LoadShaders("src/shaders/infiniteGroundVert.glsl", "src/shaders/infiniteGroundFrag.glsl");

    if (programShaderID == 0 || infiniteGroundShaderID == 0) {
        fprintf(stderr, "Failed to load shaders\n");
        return false;
    }

    infiniteGround->initGround(infiniteGroundShaderID);

    // Get uniform locations for object shader
    MatrixID = glGetUniformLocation(programShaderID, "MVP");
    ViewMatrixID = glGetUniformLocation(programShaderID, "V");
    ModelMatrixID = glGetUniformLocation(programShaderID, "M");
    LightID = glGetUniformLocation(programShaderID, "LightPosition_worldspace");
    AmbientLightID = glGetUniformLocation(programShaderID, "AmbientLightIntensity");

    // Define lights (spotlight, directional light, point light)
    setupDirectionalLight();
    setupSpotLight();
    setupPointLight();

    // Pass light uniforms
    glUseProgram(programShaderID);
    glUniform3f(AmbientLightID, ambientLightIntensity.x, ambientLightIntensity.y, ambientLightIntensity.z);

    return true;
}

void Renderer::renderScene(Model& model) {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get window size for the viewport
    int width, height;
    SDL_GetWindowSize(window.getWindow(), &width, &height);
    glViewport(0, 0, width, height);

    // Compute camera matrices
    glm::mat4 View = camera.getViewMatrix();
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

    static float rotation = 0.0f;
    rotation += 10.0f * 0.016f;
    model.setRotation(rotation, glm::vec3(0.0f, 1.0f, 0.0f));

    // Set the background (clear) color
    glm::vec3 backgroundColor(0.2f, 0.3f, 0.3f);
    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);        // Set the OpenGL clear color

    // Calculate the ground height only once, or when the model's lowest point changes
    if (!groundHeightSet || model.isLowestPointUpdateNeeded()) {
        float lowestPoint = model.getLowestPoint();                                 // Get the lowest point of the model
        std::cout << "Lowest Point: " << lowestPoint << std::endl;
        float groundHeight = lowestPoint - 0.001f;                                // Place the ground just below the object
        infiniteGround->setHeight(groundHeight);                                 // Set the ground height
        std::cout << "Ground Height Set: " << groundHeight << "\n" << std::endl;
        groundHeightSet = true;                                                // Ensure this is only done once
    }

    glUseProgram(infiniteGroundShaderID); // Render the ground
    infiniteGround->renderGround(View, Projection, directionalLights,pointLights,spotLights, backgroundColor);

    glUseProgram(programShaderID);  // Render the object

    // Model matrix and MVP
    glm::mat4 Model = model.getModelMatrix();
    glm::mat4 MVP = Projection * View * Model;

    // Pass the matrices to the shader for object rendering
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);

    renderLightsForObject();

    // Render the model
    model.draw(programShaderID);

    setupMaterialsForObject(model);

    model.updateLowestPoint();  // Update the lowest point of the model

    // Check for OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}

void Renderer::renderLightsForObject() {

    // Ensure the shader program is active
    glUseProgram(programShaderID);
    // Send the number of directional and spotlights to the shader
    glUniform1i(glGetUniformLocation(programShaderID, "numDirLights"), static_cast<GLint>(directionalLights.size()));
    glUniform1i(glGetUniformLocation(programShaderID, "numSpotLights"), static_cast<GLint>(spotLights.size()));

    // Render all directional lights
    for (int i = 0; i < static_cast<GLint>(directionalLights.size()); ++i) {
        directionalLights[i].sendToShader(programShaderID, "dirLights[" + std::to_string(i) + "]");

        directionalLights[i].enableDirectionalLights(programShaderID, i);
    }

    // Render all spot lights
    for (int i = 0; i < static_cast<GLint>(spotLights.size()); ++i) {
        spotLights[i].sendToShader(programShaderID, "spotLights[" + std::to_string(i) + "]");

        // Enable spotlight
        spotLights[i].enableSpotLights(programShaderID, i);
    }

    for (int i = 0; i < pointLights.size(); ++i) {
        pointLights[i].sendToShader(programShaderID, "pointLights[" + std::to_string(i) + "]");

        //Enable point light
    }
}

void Renderer::setupMaterialsForObject(Model& model)
{
    float materialShininess = 64.0f;  // Shiny material
    glm::vec3 materialSpecularColor = glm::vec3(1.0f, 1.0f, 1.0f);  // White specular

    glUniform3fv(glGetUniformLocation(programShaderID, "material.SpecularColor"), 1, &materialSpecularColor[0]);
    glUniform1f(glGetUniformLocation(programShaderID, "material.Shininess"), materialShininess);

    // Check if the texture exists and set the useTexture uniform
    bool textureAvailable = (model.getTextureID(0) != 0);
    glUniform1i(glGetUniformLocation(programShaderID, "useTexture"), textureAvailable);

    // Bind the diffuse texture if it exists
    if (textureAvailable) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, model.getTextureID(0));
        glUniform1i(glGetUniformLocation(programShaderID, "material.diffuseTexture"), 0);
    }
}

//---------------------Add and Get For Lights--------------------------------
std::vector<Lights>& Renderer::getPointLights() {
    return pointLights;
}

std::vector<Lights>& Renderer::getDirectionalLights() {
    return directionalLights;
}

std::vector<Lights>& Renderer::getSpotLights() {
    return spotLights;
}

void Renderer::addPointLight(const Lights& light) {
    pointLights.push_back(light);
}

void Renderer::addDirectionalLight(const Lights& light) {
    directionalLights.push_back(light);
}

void Renderer::addSpotLight(const Lights& light) {
    spotLights.push_back(light);
}
//---------------------Add and Get For Lights--------------------------------

//---------------------Default light setup--------------------------------  
void Renderer::setupDirectionalLight() {
    Lights lightDirectional(LightType::DIRECTIONAL);
    lightDirectional.setDirection(glm::vec3(-1.0f, -1.0f, -1.0f));
    lightDirectional.setIntensity(glm::vec3(2.0f, 2.0f, 2.0f));
    lightDirectional.setAmbientIntensity(glm::vec3(0.0f, 0.0f, 0.0f));      // Low ambient light
    lightDirectional.setSpecularIntensity(glm::vec3(0.05f, 0.05f, 0.05f)); // White specular light

    addDirectionalLight(lightDirectional);
}

void Renderer::setupSpotLight() {
    Lights lightSpot(LightType::SPOT);
    lightSpot.setPosition(glm::vec3(0.0f, 10.0f, 0.0f));    // Position above the object
    lightSpot.setDirection(glm::vec3(0.0f, -1.0f, 0.0f));  // Pointing downwards
    lightSpot.setIntensity(glm::vec3(2.0f, 2.0f, 2.0f));   // Intensity
    lightSpot.setCutOff(12.5f);
    lightSpot.setOuterCutOff(17.5f);
    lightSpot.setAttenuation(1.0f, 0.09f, 0.032f);
    lightSpot.setSpecularIntensity(glm::vec3(0.5f, 0.5f, 0.5f)); // White specular light

    addSpotLight(lightSpot);
}

void Renderer::setupPointLight() {
    Lights lightPoint(LightType::POINT);
    lightPoint.setPosition(glm::vec3(0.0f, 5.0f, 0.0f));   // Position above the scene
    lightPoint.setIntensity(glm::vec3(1.0f, 1.0f, 1.0f));  // Intensity
    lightPoint.setAttenuation(1.0f, 0.22f, 0.20f);         // Attenuation

    addPointLight(lightPoint);
}
//---------------------Default light setup--------------------------------

void Renderer::cleanup() {
    if (programShaderID) {
        glDeleteProgram(programShaderID);
        programShaderID = 0;
    }

    if (infiniteGroundShaderID) {
        glDeleteProgram(infiniteGroundShaderID);
        infiniteGroundShaderID = 0;
    }
}

void Renderer::setAmbientLightIntensity(const glm::vec3& intensity) {
    ambientLightIntensity = intensity;
    glUniform3f(AmbientLightID, ambientLightIntensity.x, ambientLightIntensity.y, ambientLightIntensity.z);
}

glm::vec3 Renderer::getAmbientLightIntensity() const {
    return ambientLightIntensity;
}

Window& Renderer::getWindow() {
    return window;
}

Camera& Renderer::getCamera() {
    return camera;
}