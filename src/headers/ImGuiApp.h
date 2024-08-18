#ifndef IMGUI_APP_H
#define IMGUI_APP_H


#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include "Renderer.h"
#include "Cube.h"  // Assuming you have a Cube class
#include "Window.h"
#include <SDL.h>
#include <SDL_opengl.h>
class ImGuiApp
{
public:
    ImGuiApp();
    ~ImGuiApp();
    bool Init(Window* window);
    void Run(Renderer* renderer, Cube* cube);
    void Cleanup();

private:
    bool done = false;
    glm::vec3 lightPosition = glm::vec3(4.0f, 4.0f, 4.0f);
    Window* window; // Add this member variable
};

#endif // IMGUI_APP_H
