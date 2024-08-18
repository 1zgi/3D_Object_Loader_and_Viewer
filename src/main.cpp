#include "headers/Window.h"
#include "headers/Renderer.h"
#include "headers/Cube.h"
#include "headers/ImGuiApp.h"
#include "headers/Camera.h"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

int main(int argc, char* args[]) {
    Window window(SCREEN_WIDTH, SCREEN_HEIGHT);

    Camera camera(glm::vec3(-4.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    if (!window.init()) {
        std::cerr << "Failed to initialize window\n";
        return -1;
    }

    Renderer renderer(window, camera);
    if (!renderer.init()) {
        return -1;
    }

    Cube cube;
    cube.init();

    bool running = true;
    bool mouseCapturedByImGui = false;
    SDL_Event event;

    ImGuiApp imguiApp;
    if (!imguiApp.Init(&window))
    {
        std::cerr << "Failed to initialize ImGui!" << std::endl;
        return -1;
    }

    glm::vec3 lightPos(4.0f, 4.0f, 4.0f);

    Uint32 lastTime = SDL_GetTicks();
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        while (SDL_PollEvent(&event)) {
            
            if (event.type == SDL_QUIT) {
                running = false;
            }

            // Check if ImGui is capturing the mouse
            ImGuiIO& io = ImGui::GetIO();
            mouseCapturedByImGui = io.WantCaptureMouse;

            if (!mouseCapturedByImGui && event.type == SDL_MOUSEMOTION) {
                camera.handleMouseMotion(event.motion.xrel, event.motion.yrel);
            }

            ImGui_ImplSDL2_ProcessEvent(&event);
        }
        const Uint8* state = SDL_GetKeyboardState(NULL);
        camera.handleKeyboardInput(state, deltaTime);
        
     
        imguiApp.Run(&renderer, &cube);
        
    }

    imguiApp.Cleanup();
    
    

    return 0;
}
