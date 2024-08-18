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
    positionPrinted(false){}  // Initialize the flag to false

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

    // Load shaders
    programID = LoadShaders("src/shaders/vert.glsl", "src/shaders/frag.glsl");
    if (programID == 0) {
        fprintf(stderr, "Failed to load shaders\n");
        return false;
    }

    // Get uniform locations
    MatrixID = glGetUniformLocation(programID, "MVP");
    ViewMatrixID = glGetUniformLocation(programID, "V");
    ModelMatrixID = glGetUniformLocation(programID, "M");
    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    AmbientLightID = glGetUniformLocation(programID, "AmbientLightIntensity");

    // Check uniform locations
    if (MatrixID == -1) std::cerr << "Failed to get uniform location for MVP\n";
    if (ViewMatrixID == -1) std::cerr << "Failed to get uniform location for V\n";
    if (ModelMatrixID == -1) std::cerr << "Failed to get uniform location for M\n";
    if (LightID == -1) std::cerr << "Failed to get uniform location for LightPosition_worldspace\n";
    if (AmbientLightID == -1) std::cerr << "Failed to get uniform location for AmbientLightIntensity\n";

    // If any uniform location is not found, return false
    if (MatrixID == -1 || ViewMatrixID == -1 || ModelMatrixID == -1 || LightID == -1 || AmbientLightID == -1) {
        std::cerr << "Failed to get uniform locations\n";
        return false;
    }

    // Use the shader program
    glUseProgram(programID);

    // Set the initial light properties
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(AmbientLightID, ambientLightIntensity.x, ambientLightIntensity.y, ambientLightIntensity.z);

    // Initialize VAO and VBO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Assuming some vertex data setup here...

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void Renderer::render(Cube& cube) {
    // Clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the viewport
    int width, height;
    SDL_GetWindowSize(window.getWindow(), &width, &height);
    glViewport(0, 0, width, height);

    // Compute the MVP matrix
    glm::mat4 Model = glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000.0f * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 View = camera.getViewMatrix();
    printMatrix(View);
    glm::mat4 MVP = Projection * View * Model;

    // Print the cube position
    if (!positionPrinted) {
        glm::vec4 cubePosition_worldspace = Model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);// Calculate cube's position in world space (assuming origin in model space)
        std::cout << "Cube Position in World Space: ("
            << cubePosition_worldspace.x << ", "
            << cubePosition_worldspace.y << ", "
            << cubePosition_worldspace.z << ")" << std::endl;
        positionPrinted = true;  // Set the flag to true
    }

    // Pass the MVP matrix to the shader
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
    glUseProgram(programID);

    // Update the light position in the shader
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

    // Render the cube
    cube.draw();

    // Check for OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}

void Renderer::cleanup() {
    // Cleanup shaders
    if (programID) {
        glDeleteProgram(programID);
        programID = 0;
    }

    // Cleanup VAO and VBO
    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }

    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    // Any additional cleanup for OpenGL resources
}

void Renderer::printMatrix(const glm::mat4& matrix) {
    std::cout << "Matrix:\n";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
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
