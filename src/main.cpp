#include "headers/Window.h"
#include "headers/Renderer.h"
#include "headers/ImGuiApp.h"
#include "headers/Camera.h"
#include "headers/Model.h" 

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

int main(int argc, char* args[]) {
    Window window(SCREEN_WIDTH, SCREEN_HEIGHT);

    Camera camera(glm::vec3(-2.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    if (!window.init()) {
        std::cerr << "Failed to initialize window\n";
        return -1;
    }

    Model model("models/obj/Earth.obj");

    Renderer renderer(window, camera, model);
    if (!renderer.init()) {
        return -1;
    }

    bool running = true;
    bool mouseCapturedByImGui = false;
    SDL_Event event;

    ImGuiApp imguiApp;
    if (!imguiApp.Init(&window))
    {
        std::cerr << "Failed to initialize ImGui!" << std::endl;
        return -1;
    }

    Uint32 lastTime = SDL_GetTicks();
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // Initialize io before the switch statement to avoid skipping it
        ImGuiIO& io = ImGui::GetIO();
        mouseCapturedByImGui = io.WantCaptureMouse;

        while (SDL_PollEvent(&event)) {

            switch (event.type) {
            case SDL_QUIT:
                std::cout << "SDL_QUIT received" << std::endl;
                running = false;
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                    std::cout << "SDL_WINDOWEVENT_CLOSE received" << std::endl;
                    running = false;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (!mouseCapturedByImGui && event.button.button == SDL_BUTTON_LEFT) {
                    camera.handleMouseButton(true);  // Start rotating camera when left mouse button is pressed
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (!mouseCapturedByImGui && event.button.button == SDL_BUTTON_LEFT) {
                    camera.handleMouseButton(false);  // Stop rotating camera when left mouse button is released
                }
                break;

            case SDL_MOUSEMOTION:
                if (!mouseCapturedByImGui && camera.isMouseHeld()) {  // Only rotate when the mouse button is held down
                    camera.handleMouseMotion(event.motion.xrel, event.motion.yrel);  // Pass relative mouse movement
                }
                break;

            case SDL_MOUSEWHEEL:
                 camera.handleMouseScroll(static_cast<float>(event.wheel.y));  // y is positive for zoom in, negative for zoom out
                break; 
            }

            ImGui_ImplSDL2_ProcessEvent(&event);
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);

        // Render the model with the renderer
        renderer.renderScene();

        // Run ImGui interface
        imguiApp.Run(&renderer, &model);
    }

    imguiApp.Cleanup();
    
    window.swapBuffers();

    return 0;
}
