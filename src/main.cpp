#include "headers/Window.h"
#include "headers/Renderer.h"
#include "headers/ImGuiApp.h"
#include "headers/Camera.h"
#include "headers/Model.h"
#include <iostream>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#undef byte  // Prevent conflicts with std::byte
#endif

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

// Function to show file dialog and get OBJ file path
std::string showFileDialog() {
#ifdef _WIN32
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "OBJ Files\0*.obj\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    ofn.lpstrTitle = "Select OBJ Model File";
    
    if (GetOpenFileNameA(&ofn)) {
        return std::string(szFile);
    }
#endif
    return "";
}

int main(int argc, char* args[]) {
    std::string modelPath;
    bool hasInitialModel = false;
    
    // Check if a file path was provided as command line argument
    if (argc >= 2) {
        modelPath = args[1];
        
        // Verify the file exists and has .obj extension
        if (modelPath.length() >= 4 && modelPath.substr(modelPath.length() - 4) == ".obj") {
            std::cout << "Loading model from command line: " << modelPath << std::endl;
            hasInitialModel = true;
        } else {
            std::cerr << "Error: Please provide a valid .obj file!" << std::endl;
            std::cerr << "Usage: " << args[0] << " [path_to_model.obj]" << std::endl;
            return -1;
        }
    } else {
        std::cout << "Starting 3D Object Loader and Viewer..." << std::endl;
        std::cout << "Use 'Browse Models...' to load an OBJ file, or drag and drop a file into the window." << std::endl;
    }

    Window window(SCREEN_WIDTH, SCREEN_HEIGHT);
    Camera camera(glm::vec3(-3.0f, 2.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    if (!window.init()) {
        std::cerr << "Failed to initialize window\n";
        return -1;
    }

    // Enable drag and drop
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    // Create model - either with initial file or empty
    Model model(hasInitialModel ? modelPath : "");

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
                
            case SDL_DROPFILE:
                {
                    char* droppedFile = event.drop.file;
                    std::string filePath(droppedFile);
                    
                    std::cout << "File dropped: " << filePath << std::endl;
                    
                    // Check if it's an OBJ file
                    if (filePath.size() > 4 && filePath.substr(filePath.size() - 4) == ".obj") {
                        if (model.reloadModel(filePath)) {
                            std::cout << "Model loaded successfully via drag & drop!" << std::endl;
                        } else {
                            std::cerr << "Failed to load dropped model!" << std::endl;
                        }
                    } else {
                        std::cout << "Dropped file is not an OBJ file" << std::endl;
                    }
                    
                    SDL_free(droppedFile);  // Free the memory
                }
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
