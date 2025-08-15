#ifndef IMGUI_APP_H
#define IMGUI_APP_H


#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <string>
#include <vector>
#include "Renderer.h"
#include "Window.h"
#include <SDL.h>
#include <SDL_opengl.h>
class ImGuiApp
{
public:
    ImGuiApp();
    ~ImGuiApp();
    bool Init(Window* window);
    void Run(Renderer* renderer, Model* model);
    void Cleanup();

private:
    bool done = false;
    glm::vec3 lightPosition = glm::vec3(4.0f, 4.0f, 4.0f);
    Window* window; //member variable
    
    // File browser state
    bool showFileBrowser = false;
    std::string currentDirectory;
    std::vector<std::string> directoryContents;
    std::string selectedFile;
    std::string loadingStatus;
    bool isLoading = false;
    
    // Helper methods for file browser
    std::string showFileDialog();
    void renderFileBrowser(Model* model);
    void refreshDirectoryContents();
    bool isObjFile(const std::string& filename);
    std::string getFileExtension(const std::string& filename);
};

#endif // IMGUI_APP_H
