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
    dirLightIntensity (1.0f, 1.0f, 1.0f),
    dirLightColor(0),
    lightPos(4.0f, 4.0f, 4.0f),
    lightIntensity(1.0f, 1.0f, 1.0f),
    ambientLightIntensity(0.2f, 0.2f, 0.2f),
    vao(0),
    vbo(0),
    infiniteGround(std::make_unique<InfiniteGround>()),
    positionPrinted(false){}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::init() {
    // Initialize OpenGL, shaders, and GLEW (as before)
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return false;
    }

    // Set the background (clear) color here
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    programID = LoadShaders("src/shaders/vert.glsl", "src/shaders/frag.glsl");
    infiniteGroundShader = LoadShaders("src/shaders/infiniteGroundVert.glsl", "src/shaders/infiniteGroundFrag.glsl");

    if (programID == 0 || infiniteGroundShader == 0) {
        fprintf(stderr, "Failed to load shaders\n");
        return false;
    }

    MatrixID = glGetUniformLocation(programID, "MVP");
    ViewMatrixID = glGetUniformLocation(programID, "V");
    ModelMatrixID = glGetUniformLocation(programID, "M");
    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    AmbientLightID = glGetUniformLocation(programID, "AmbientLightIntensity");

    if (MatrixID == -1 || ViewMatrixID == -1 || ModelMatrixID == -1 || LightID == -1 || AmbientLightID == -1) {
        std::cerr << "Failed to get uniform locations\n";
        return false;
    }

    glUseProgram(programID);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(AmbientLightID, ambientLightIntensity.x, ambientLightIntensity.y, ambientLightIntensity.z);

    return true;
}

void Renderer::addObject(std::shared_ptr<Model> obj) {
    objects.push_back(obj);  // Add the object to the render list
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

    // Set the background (clear) color
    glm::vec3 backgroundColor(0.2f, 0.3f, 0.3f);  // Example background color
    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);  // Set the OpenGL clear color

    // In your rendering function, use this color for the ground
    glUseProgram(infiniteGroundShader);

    // Set the background color uniform in the ground shader
    GLuint backgroundColorLocation = glGetUniformLocation(infiniteGroundShader, "backgroundColor");
    glUniform3fv(backgroundColorLocation, 1, &backgroundColor[0]);

    // Render the ground
    glUseProgram(infiniteGroundShader);
    infiniteGround->renderGround(infiniteGroundShader, View, Projection);

    // Use the program for objects
    glUseProgram(programID);

    static float rotation = 0.0f;
    rotation += 10.0f * 0.016f;
    model.setRotation(rotation, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 Model = model.getModelMatrix();
    glm::mat4 MVP = Projection * View * Model;

    // Set shader uniforms for the object
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);

    // Set directional light properties
    glUniform3f(glGetUniformLocation(programID, "dirLight.Intensity"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(programID, "dirLight.Direction"), -1.0f, -1.0f, -1.0f);

    // Set the point light properties
    glm::vec3 pointLightPosition(5.0f, 5.0f, 5.0f); // Example position of the point light
    glUniform3f(glGetUniformLocation(programID, "pointLight.Intensity"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(programID, "pointLight.Position"), pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);

    // Set the point light attenuation
    glUniform1f(glGetUniformLocation(programID, "pointLight.Constant"), 1.0f);
    glUniform1f(glGetUniformLocation(programID, "pointLight.Linear"), 0.09f);
    glUniform1f(glGetUniformLocation(programID, "pointLight.Quadratic"), 0.032f);

    size_t materialIndex = 0;
    // Set the diffuse color from the material (loaded from the MTL file)
    glm::vec3 materialDiffuseColor = model.getMaterialDiffuseColor(materialIndex);  // This should return the diffuse color from the material
    glUniform3fv(glGetUniformLocation(programID, "material.DiffuseColor"), 1, &materialDiffuseColor[0]);

    // Check if the texture exists and set the useTexture uniform
    bool textureAvailable = (model.getTextureID(0) != 0);
    glUniform1i(glGetUniformLocation(programID, "useTexture"), textureAvailable);

    // Bind the diffuse texture if it exists
    if (textureAvailable) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, model.getTextureID(0));
        glUniform1i(glGetUniformLocation(programID, "material.diffuseTexture"), 0);  // Set the texture unit
    }

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, model.getSpecularTextureID(0));  // Specular texture

    // Print model position in world space once
    if (!positionPrinted) {
        glm::vec4 modelPosition_worldspace = Model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        std::cout << "\nModel Position in World Space: ("
            << modelPosition_worldspace.x << ", "
            << modelPosition_worldspace.y << ", "
            << modelPosition_worldspace.z << ")\n" << std::endl;
        positionPrinted = true;
    }

    model.draw(programID);

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

void Renderer::setAmbientLightIntensity(const glm::vec3& intensity){
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

