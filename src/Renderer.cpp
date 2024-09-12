#include "headers/Renderer.h"

Renderer::Renderer(Window& window, Camera& camera)
    : window(window),
    camera(camera),
    programID(0),
    infiniteGroundShader(0),
    MatrixID(0),
    ViewMatrixID(0),
    ModelMatrixID(0),
    LightID(0),
    AmbientLightID(0),
    Projection(glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
    dirLightDirection(-1.0f, -1.0f, -1.0f),
    dirLightIntensity(1.0f, 1.0f, 1.0f),
    dirLightColor(0),
    lightPos(4.0f, 4.0f, 4.0f),
    lightIntensity(1.0f, 1.0f, 1.0f),
    ambientLightIntensity(0.2f, 0.2f, 0.2f),
    vao(0),
    vbo(0),
    infiniteGround(std::make_unique<InfiniteGround>()),
    positionPrinted(false),
    groundHeightSet(false) {}

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
    programID = LoadShaders("src/shaders/vert.glsl", "src/shaders/frag.glsl");
    infiniteGroundShader = LoadShaders("src/shaders/infiniteGroundVert.glsl", "src/shaders/infiniteGroundFrag.glsl");

    if (programID == 0 || infiniteGroundShader == 0) {
        fprintf(stderr, "Failed to load shaders\n");
        return false;
    }

    infiniteGround->initGround(infiniteGroundShader);

    // Get uniform locations for object shader
    MatrixID = glGetUniformLocation(programID, "MVP");
    ViewMatrixID = glGetUniformLocation(programID, "V");
    ModelMatrixID = glGetUniformLocation(programID, "M");
    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    AmbientLightID = glGetUniformLocation(programID, "AmbientLightIntensity");

    // Pass light uniforms
    glUseProgram(programID);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(AmbientLightID, ambientLightIntensity.x, ambientLightIntensity.y, ambientLightIntensity.z);

    return true;
}

void Renderer::render(Model& model) {
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

    // Define your lights (spotlight, directional light, point light)
    Lights directionalLight(LightType::DIRECTIONAL);
    directionalLight.setDirection(glm::vec3(-1.0f, -1.0f, -1.0f));
    directionalLight.setIntensity(glm::vec3(2.0f, 2.0f, 2.0f));
    directionalLight.setAmbientIntensity(glm::vec3(0.0f, 0.0f, 0.0f));      // Low ambient light
    directionalLight.setSpecularIntensity(glm::vec3(0.05f, 0.05f, 0.05f)); // White specular light

    Lights pointLight(LightType::POINT);
    pointLight.setPosition(glm::vec3(0.0f, 5.0f, 0.0f));   // Position above the scene
    pointLight.setIntensity(glm::vec3(1.0f, 1.0f, 1.0f)); // Intensity
    pointLight.setAttenuation(1.0f, 0.22f, 0.20f);       // Attenuation

    Lights spotLight(LightType::SPOT);
    spotLight.setPosition(glm::vec3(0.0f, 10.0f, 0.0f));    // Position above the object
    spotLight.setDirection(glm::vec3(0.0f, -1.0f, 0.0f));  // Pointing downwards
    spotLight.setIntensity(glm::vec3(2.0f, 2.0f, 2.0f));  // Intensity
    spotLight.setCutOff(12.5f);
    spotLight.setOuterCutOff(17.5f);
    spotLight.setAttenuation(1.0f, 0.09f, 0.032f);
    spotLight.setSpecularIntensity(glm::vec3(0.5f, 0.5f, 0.5f)); // White specular light

    // Render the ground
    glUseProgram(infiniteGroundShader);
    infiniteGround->renderGround(View, Projection, directionalLight, pointLight, spotLight, backgroundColor);

    // Render the object
    glUseProgram(programID);

    // Model matrix and MVP
    glm::mat4 Model = model.getModelMatrix();
    glm::mat4 MVP = Projection * View * Model;

    // Pass the matrices to the shader for object rendering
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);

    // Enable directional light
    glUniform1i(glGetUniformLocation(programID, "useDirectionalLight"), true);

    // Enable spotlight
    glUniform1i(glGetUniformLocation(programID, "useSpotLight"), true);

    // Pass the light data to both object and ground shaders
    directionalLight.sendToShader(programID, "dirLight");
    pointLight.sendToShader(programID, "pointLight");
    spotLight.sendToShader(programID, "spotLight");
    
    // Set material properties
    size_t materialIndex = 0;
    glm::vec3 materialDiffuseColor = model.getMaterialDiffuseColor(materialIndex);
    glUniform3fv(glGetUniformLocation(programID, "material.DiffuseColor"), 1, &materialDiffuseColor[0]);

    float materialShininess = 35.0f;  // Shiny material
    glm::vec3 materialSpecularColor = glm::vec3(1.0f, 1.0f, 1.0f);  // White specular

    glUniform3fv(glGetUniformLocation(programID, "materialSpecularColor"), 1, &materialSpecularColor[0]);
    glUniform1f(glGetUniformLocation(programID, "materialShininess"), materialShininess);
    // Check if the texture exists and set the useTexture uniform

    bool textureAvailable = (model.getTextureID(0) != 0);
    glUniform1i(glGetUniformLocation(programID, "useTexture"), textureAvailable);

    // Bind the diffuse texture if it exists
    if (textureAvailable) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, model.getTextureID(0));
        glUniform1i(glGetUniformLocation(programID, "material.diffuseTexture"), 0);
    }

    // Render the model
    model.draw(programID);

    model.updateLowestPoint();  // Update the lowest point of the model

    // Check for OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}

void Renderer::cleanup() {
    if (programID) {
        glDeleteProgram(programID);
        programID = 0;
    }

    if (infiniteGroundShader) {
        glDeleteProgram(infiniteGroundShader);
        infiniteGroundShader = 0;
    }
}

void Renderer::setLightPosition(const glm::vec3& position) {
    lightPos = position;
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
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
