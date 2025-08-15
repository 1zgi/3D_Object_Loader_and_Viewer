#include "headers/Renderer.h"
#include <SDL.h>

Renderer::Renderer(Window& window, Camera& camera, Model& model)
    : window(window),
    camera(camera),
    model(model),
    shadowMap(2048, 2048),
    programShaderID(0),
    infiniteGroundShaderID(0),
    shadowMapShaderID(0),
    MatrixID(0),
    ViewMatrixID(0),
    ModelMatrixID(0),
    LightID(0),
    AmbientLightID(0),
    Projection(glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
    ambientLightIntensity(0.5f, 0.5f, 0.5f),
    vao(0),
    vbo(0),
    lightPos(0.0f, 0.0f, 0.0f),
    targetPos(0.0f, 0.0f, 0.0f),
    infiniteGround(std::make_unique<InfiniteGround>()),
    positionPrinted(false),
    groundHeightSet(false),
    autoRotateModel(true),
    rotationSpeed(30.0f),
    shadowsEnabled(true){}

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
    shadowMapShaderID = LoadShaders("src/shaders/shadowVert.glsl", "src/shaders/shadowFrag.glsl");

    if (programShaderID == 0 || infiniteGroundShaderID == 0 || shadowMapShaderID == 0) {
        fprintf(stderr, "Failed to load shaders\n"); 
        return false;
    }

    infiniteGround->initGround(infiniteGroundShaderID);

    MatrixUniformLocations(programShaderID);

    // Define lights (spotlight, directional light, point light)
    setupDirectionalLight();
    setupSpotLight();
    setupPointLight();

    shadowMap.init();

    // Pass light uniforms
    glUseProgram(programShaderID);
    glUniform3f(AmbientLightID, ambientLightIntensity.x, ambientLightIntensity.y, ambientLightIntensity.z);

    return true;
}



// Function to set the shadow matrix uniform in the shader
void Renderer::setShadowMatrixUniform(GLuint shaderID, const glm::mat4& shadowMatrix) {
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "matrixShadow"), 1, GL_FALSE, &shadowMatrix[0][0]);
}

// Function to bind the shadow map to a texture unit
void Renderer::bindShadowMap(GLuint shaderID, GLuint textureUnit) {
    shadowMap.bindForLightingPass(textureUnit);
    glUniform1i(glGetUniformLocation(shaderID, "shadowMap"), textureUnit);
}

// Render ground with shadows
void Renderer::renderGroundWithShadows(const glm::mat4& view, const glm::mat4& projection) {
    // Render the ground with the shadow map applied
    infiniteGround->renderGround(infiniteGroundShaderID, view, projection, directionalLights, pointLights, spotLights,
        glm::vec3(0.2f, 0.3f, 0.3f), shadowMap, shadowsEnabled);
}

// Render model with shadows
void Renderer::renderModelWithShadows(const glm::mat4& View, const glm::mat4& Projection) {
    glUseProgram(programShaderID);
    MatrixUniformLocations(programShaderID);

    // Get the light space matrix from the ShadowMap class
    glm::mat4 lightSpaceMatrix = shadowMap.getLightSpaceMatrix();

    // Compute the model matrix and MVP matrix
    glm::mat4 Model = model.getModelMatrix();
    glm::mat4 MVP = calculateMVP(Model, View, Projection);

    // Pass the matrices to the object shader
    MatrixPassToShader(MVP, View, Model);

    // Pass the light space matrix to the vertex shader
    glUniformMatrix4fv(glGetUniformLocation(programShaderID, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);

    // Set shadow bias uniform for the fragment shader (increased for better self-shadow prevention)
    glUniform1f(glGetUniformLocation(programShaderID, "shadowBias"), 0.01f);

    // Set shadows enabled uniform
    glUniform1i(glGetUniformLocation(programShaderID, "shadowsEnabled"), shadowsEnabled ? 1 : 0);

    // Update ambient light intensity uniform each frame
    glUniform3f(glGetUniformLocation(programShaderID, "AmbientLightIntensity"), 
                ambientLightIntensity.x, ambientLightIntensity.y, ambientLightIntensity.z);

    // Bind the shadow map texture
    bindShadowMap(programShaderID, 1);

    // Render the model
    model.draw(programShaderID);
}

// Render scene
void Renderer::renderScene() {
    // First pass: Render to the shadow map (only if shadows are enabled)
    if (shadowsEnabled) {
        renderToTheDepthTexture();
    }

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set viewport
    int width, height;
    SDL_GetWindowSize(window.getWindow(), &width, &height);
    glViewport(0, 0, width, height);
    
    // Ensure proper OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);  // Enable face culling by default

    glm::mat4 View = camera.getViewMatrix();
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

    // Apply automatic rotation to showcase dynamic shadows
    if (autoRotateModel) {
        static Uint32 lastTime = SDL_GetTicks();
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // Update model rotation around Y-axis
        static float currentRotation = 0.0f;
        currentRotation += rotationSpeed * deltaTime;
        if (currentRotation >= 360.0f) {
            currentRotation -= 360.0f;
        }
        model.setRotation(currentRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    if (!groundHeightSet || model.isLowestPointUpdateNeeded()) {
        float lowestPoint = model.getLowestPoint();
        float groundHeight = lowestPoint - 0.001f;
        infiniteGround->setHeight(groundHeight);
        groundHeightSet = true;
    }

    // Render the lights for the object
    renderLightsForObject();  // Ensure the lights are set up for the object 

    renderModelWithShadows(View, Projection);
    // Render the ground and model with shadows
    renderGroundWithShadows(View, Projection);
    
   
    // Update model's lowest point for the next frame
    model.updateLowestPoint();

    // Check for OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}

// Render to the depth texture (shadow map)
void Renderer::renderToTheDepthTexture() {
    shadowMap.bindForShadowPass();  // Bind shadow framebuffer
    glUseProgram(shadowMapShaderID);  // Use shadow shader

    // Calculate light space matrix using the first directional light
    if (!directionalLights.empty()) {
        glm::vec3 lightPos = -directionalLights[0].getDirection() * 10.0f;  // Position light far away in opposite direction
        glm::vec3 targetPos = glm::vec3(0.0f, 0.0f, 0.0f);  // Point towards scene center
        shadowMap.calculateLightSpaceMatrix(lightPos, targetPos, directionalLights[0].getLightType());
    }

    // Get the calculated light space matrix
    glm::mat4 lightSpaceMatrix = shadowMap.getLightSpaceMatrix();
    
    // Extract light projection and view from the light space matrix
    // For orthographic projection for directional light (smaller bounds for tighter shadows)
    glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 50.0f);
    glm::vec3 lightPos = -directionalLights[0].getDirection() * 10.0f;
    glm::vec3 targetPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 lightView = glm::lookAt(lightPos, targetPos, glm::vec3(0.0f, 1.0f, 0.0f));

    // ---- Render Model to Shadow Map ----
    glm::mat4 modelMatrix = model.getModelMatrix();
    glm::mat4 lightMVP = lightProjection * lightView * modelMatrix;

    // Pass light-space transformation to the shadow shader for the model
    glUniformMatrix4fv(glGetUniformLocation(shadowMapShaderID, "lightMVP"), 1, GL_FALSE, &lightMVP[0][0]);
    
    // Pass the shadow matrix for texture sampling
    glm::mat4 shadowMatrix = lightSpaceMatrix * modelMatrix;
    glUniformMatrix4fv(glGetUniformLocation(shadowMapShaderID, "matrixShadow"), 1, GL_FALSE, &shadowMatrix[0][0]);

    // Render the model into the shadow map
    model.draw(shadowMapShaderID);

    shadowMap.bindForCameraView();  // Rebind framebuffer for regular camera rendering
}




void Renderer::renderLightsForObject() {

    // Ensure the shader program is active
    glUseProgram(programShaderID);
    // Send the number of directional and spotlights to the shader
    glUniform1i(glGetUniformLocation(programShaderID, "numDirLights"), static_cast<GLint>(directionalLights.size()));
    glUniform1i(glGetUniformLocation(programShaderID, "numSpotLights"), static_cast<GLint>(spotLights.size()));

    // Render all directional lights
    for (int i = 0; i < static_cast<GLint>(directionalLights.size()); ++i) {

        lightPos = -directionalLights[i].getDirection() * 10.0f;  // Simulate a far-away light source
        targetPos = glm::vec3(0.0f, 0.0f, 0.0f);  // Light is pointing towards the center of the scene (or the object)

        // Update shadow map for the first directional light (primary shadow caster)
        if (i == 0) {
            shadowMap.calculateLightSpaceMatrix(lightPos, targetPos, directionalLights[i].getLightType());
        }

        directionalLights[i].sendToShader(programShaderID, "dirLights[" + std::to_string(i) + "]");
        directionalLights[i].enableDirectionalLights(programShaderID, i);
   
    }

    // Render all spot lights
    for (int i = 0; i < static_cast<GLint>(spotLights.size()); ++i) {
        lightPos = spotLights[i].getPosition();  // Use actual spotlight position
        targetPos = lightPos + spotLights[i].getDirection();  // Light is pointing in its direction

        // Update shadow map for spot lights if no directional light is present
        if (directionalLights.empty() && i == 0) {
            shadowMap.calculateLightSpaceMatrix(lightPos, targetPos, spotLights[i].getLightType());
        }

        spotLights[i].sendToShader(programShaderID, "spotLights[" + std::to_string(i) + "]");
        spotLights[i].enableSpotLights(programShaderID, i);
       
    }

    for (int i = 0; i < pointLights.size(); ++i) {
        lightPos = pointLights[i].getPosition();  // Use actual point light position
        targetPos = glm::vec3(0.0f, 0.0f, 0.0f);  // Point light points toward scene center

        // Update shadow map for point lights if no directional or spot lights are present
        if (directionalLights.empty() && spotLights.empty() && i == 0) {
            shadowMap.calculateLightSpaceMatrix(lightPos, targetPos, pointLights[i].getLightType());
        }

        pointLights[i].sendToShader(programShaderID, "pointLights[" + std::to_string(i) + "]");
        pointLights[i].enablePointLights(programShaderID, i);
        
    }
}

glm::mat4 Renderer::calculateMVP(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    return projectionMatrix * viewMatrix * modelMatrix;
}


void Renderer:: MatrixUniformLocations(GLuint programShaderID)
{
    // Get uniform locations for object shader
    MatrixID = glGetUniformLocation(programShaderID, "MVP");
    ViewMatrixID = glGetUniformLocation(programShaderID, "V");
    ModelMatrixID = glGetUniformLocation(programShaderID, "M");
    LightID = glGetUniformLocation(programShaderID, "LightPosition_worldspace");
    AmbientLightID = glGetUniformLocation(programShaderID, "AmbientLightIntensity");
}

void Renderer::MatrixPassToShader(const glm::mat4& MVP, const glm::mat4& View, const glm::mat4& Model)
{
    // Pass the matrices to the shader for object rendering
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
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

    if (shadowMapShaderID) {
        glDeleteProgram(shadowMapShaderID);
        shadowMapShaderID = 0;
    }
}

void Renderer::setModel(const Model& model) {
    this->model = model;
}

void Renderer::setAmbientLightIntensity(const glm::vec3& intensity) {
    ambientLightIntensity = intensity;
    // Ambient light uniform will be updated in renderModelWithShadows() during rendering
}

glm::vec3 Renderer::getAmbientLightIntensity() const {
    return ambientLightIntensity;
}

void Renderer::setAutoRotation(bool enabled) {
    autoRotateModel = enabled;
}

bool Renderer::getAutoRotation() const {
    return autoRotateModel;
}

void Renderer::setRotationSpeed(float speed) {
    rotationSpeed = speed;
}

void Renderer::setShadowsEnabled(bool enabled) {
    shadowsEnabled = enabled;
}

bool Renderer::getShadowsEnabled() const {
    return shadowsEnabled;
}

Window& Renderer::getWindow() {
    return window;
}

Camera& Renderer::getCamera() {
    return camera;
}