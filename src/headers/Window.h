#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>
#include <stdio.h>

class Window {
public:
    Window(int width, int height);
    ~Window();
    bool init();
    SDL_Window* getWindow();
    SDL_GLContext getContext();
private:
    int width;
    int height;
    SDL_Window* window;
    SDL_GLContext context;
};

#endif
