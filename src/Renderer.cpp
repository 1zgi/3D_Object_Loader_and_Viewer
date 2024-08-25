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

    // Rotate the model around its origin
    static float rotation = 0.0f;
    rotation += 10.0f * 0.016f;  // Decrease the rotation speed to 10 degrees per second
    model.setRotation(rotation, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 Model = model.getModelMatrix();
    glm::mat4 View = camera.getViewMatrix();
    glm::mat4 MVP = Projection * View * Model;

    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);

    if (!positionPrinted) {
        glm::vec4 modelPosition_worldspace = Model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        std::cout << "Model Position in World Space: ("
            << modelPosition_worldspace.x << ", "
            << modelPosition_worldspace.y << ", "
            << modelPosition_worldspace.z << ")" << std::endl;
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
    glUseProgram(programID);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
}

Window& Renderer::getWindow() {
    return window;
}
