#include "headers/Window.h"
#include <GL/glew.h>
#include <iostream>

Window::Window(int width, int height)
    : width(width), height(height), window(nullptr), context(nullptr) {}

Window::~Window() {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Window::init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    // Set SDL OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);   // OpenGL version 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);            // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);             // Depth buffer size

    // Create the SDL window with OpenGL context
    window = SDL_CreateWindow("ViewMe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    // Create the OpenGL context
    context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        fprintf(stderr, "Failed to create OpenGL context: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // Initialize GLEW after the context is created
    glewExperimental = GL_TRUE;  // Enable modern OpenGL function loading
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // V-Sync
    if (SDL_GL_SetSwapInterval(1) < 0) {  // 1 enables V-Sync
        std::cerr << "Warning: Unable to set V-Sync: " << SDL_GetError() << std::endl;
    }

    return true;
}

SDL_Window* Window::getWindow() {
    return window;
}

SDL_GLContext Window::getContext() {
    return context;
}

void Window::swapBuffers() {
    SDL_GL_SwapWindow(window);  // Swap buffers to display the rendered frame
}
