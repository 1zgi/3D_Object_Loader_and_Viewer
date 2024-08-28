#include "headers/Renderer.h"

Renderer::Renderer(Window& window, Camera& camera)
    : window(window),
    camera(camera),
    programID(0),
    MatrixID(0),
    ViewMatrixID(0),
    ModelMatrixID(0),
    LightID(0),
    AmbientLightID(0),
    Projection(glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f)),
    lightPos(4.0f, 4.0f, 4.0f),
    lightIntensity(1.0f, 1.0f, 1.0f),
    ambientLightIntensity(0.2f, 0.2f, 0.2f),
    vao(0),
    vbo(0),
    positionPrinted(false) {}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::init() {
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
    if (programID == 0) {
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

void Renderer::render(Model& model) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int width, height;
    SDL_GetWindowSize(window.getWindow(), &width, &height);
    glViewport(0, 0, width, height);

    static float rotation = 0.0f;
    rotation += 10.0f * 0.016f;
    model.setRotation(rotation, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 Model = model.getModelMatrix();
    glm::mat4 View = camera.getViewMatrix();
    glm::mat4 MVP = Projection * View * Model;

    glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programID, "ModelMatrix"), 1, GL_FALSE, &Model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programID, "ViewMatrix"), 1, GL_FALSE, &View[0][0]);

    // Set the directional light properties
    glUniform3f(glGetUniformLocation(programID, "dirLight.Intensity"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(programID, "dirLight.Direction"), -1.0f, -1.0f, -1.0f);

    // Set the point light properties
    glm::vec3 pointLightPosition(5.0f, 5.0f, 5.0f); // Example position of the point light
    glUniform3f(glGetUniformLocation(programID, "pointLight.Intensity"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(programID, "pointLight.Position"), pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);

    // Set the material properties
    glUniform3f(glGetUniformLocation(programID, "material.SpecularColor"), 1.0f, 1.0f, 1.0f);
    glUniform1i(glGetUniformLocation(programID, "material.diffuseTexture"), 0);
    glUniform1i(glGetUniformLocation(programID, "material.specularTexture"), 1);

    glUniform1f(glGetUniformLocation(programID, "pointLight.Constant"), 1.0f);
    glUniform1f(glGetUniformLocation(programID, "pointLight.Linear"), 0.09f);
    glUniform1f(glGetUniformLocation(programID, "pointLight.Quadratic"), 0.032f); 

    // Set the camera position
    glm::vec3 cameraPos = camera.getPosition();
    glUniform3f(glGetUniformLocation(programID, "gCameraLocalPos"), cameraPos.x, cameraPos.y, cameraPos.z);

    // Bind the textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model.getTextureID(0));  // Diffuse texture

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

    model.draw();

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
}

void Renderer::setLightPosition(const glm::vec3& position) {
    lightPos = position;
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
}

Window& Renderer::getWindow() {
    return window;
}

Camera& Renderer::getCamera() {
    return camera;
}

