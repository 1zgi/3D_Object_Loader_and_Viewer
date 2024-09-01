#include "headers/ImGuiApp.h"

ImGuiApp::ImGuiApp() : window(nullptr), done(false) {}

ImGuiApp::~ImGuiApp() {
    Cleanup();
}

bool ImGuiApp::Init(Window* window) {
    this->window = window;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    if (!ImGui_ImplSDL2_InitForOpenGL(window->getWindow(), window->getContext())) {
        std::cerr << "Failed to initialize ImGui SDL2 backend!" << std::endl;
        return false;
    }
    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        std::cerr << "Failed to initialize ImGui OpenGL3 backend!" << std::endl;
        return false;
    }

    return true;
}

void ImGuiApp::Run(Renderer* renderer, Model* model) {
   
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    glm::vec3 currentAmbientIntensity = renderer->getAmbientLightIntensity();

    // Calculate the scalar intensity as the average of the components(or use length for different logic)
    float ambientIntensity = (currentAmbientIntensity.x + currentAmbientIntensity.y + currentAmbientIntensity.z) / 3.0f;

    // Add controls for ambient light intensity
    ImGui::Begin("Ambient Light Control");
    if (ImGui::SliderFloat("Ambient Intensity", &ambientIntensity, 0.2f, 1.0f)) {
      // Update ambientLightIntensity uniformly with the new value
      renderer->setAmbientLightIntensity(glm::vec3(ambientIntensity));
    }
    ImGui::End();

   // Update the light position in the renderer
   renderer->setLightPosition(lightPosition);

   // Render your scene
   renderer->render(*model);

   // Rendering ImGui
   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

   // Swap buffers
   SDL_GL_SwapWindow(renderer->getWindow().getWindow());

   // Check for OpenGL errors
   GLenum err;
   while ((err = glGetError()) != GL_NO_ERROR) {
      std::cerr << "OpenGL error: " << err << std::endl;
   } 
}

void ImGuiApp::Cleanup() {
    // Cleanup ImGui
    if (ImGui::GetCurrentContext()) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    // Note: Cleanup SDL only if this class is responsible for it.
    if (window) {
        // Avoid deleting the SDL context and window if they're managed elsewhere
        window = nullptr;
    }

    // Avoid calling SDL_Quit here if it's managed elsewhere in your application.
    // SDL_Quit();
}
