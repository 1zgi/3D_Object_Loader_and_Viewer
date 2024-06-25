//Third Party Libraries
#include <glad/glad.h>
#include <SDL.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

//c++ libs (STL)
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

//Headers
#include "headers/ClassShader.h"
#include "headers/VAO.h"
#include "headers/VAO.h"
#include "headers/EBO.h"




#undef main
using namespace std;

//Globals (generally are prefixed with 'g' in this application)

//Screen Dimensions (Window)
int gScreenWidth = 640;
int gScreenHeight = 480;
SDL_Window* gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;

//Main loop flag
bool gQuit = false; // if true, we quit

//Some Info's
void GetOpenGLVersionInfo()
{
    cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "Version: " << glGetString(GL_VERSION) << endl;
    cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

void InitalizeProgram()
{
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "SDL2 could not initialize video subsystem" << endl;
        exit(1);
    }

    //Setup the OpenGL Context
    //Use OpenGL 4.1 core or greater
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);//using OpenGL 4.1 (Available for mac, windows and linux)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);//NOTE: Mac unfortunatelly doesn't support beyond version 4.1 (can be try version 3.3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    //we want to request a double buffer for smooth updating
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //Creating an application window using OpenGL that supports SDL
    gGraphicsApplicationWindow = SDL_CreateWindow("3D Asset Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED
        , gScreenWidth, gScreenHeight
        , SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    //check if Window did not create
    if (gGraphicsApplicationWindow == nullptr)
    {
        cout << "SDL_Window was not able to be created" << endl;

        exit(1);
    }

    // Create an OpenGL Graphics Context
    gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);
    if (gOpenGLContext == nullptr)
    {
        cout << "OpenGL context not available\n";
        exit(1);
    }

    //Initialize GLAD Library
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        cout << "glad was not initialized" << endl;
        exit(1);
    }
   

    GetOpenGLVersionInfo();
}

void Input()
{
    SDL_Event e;//Handles various events in SDL that are related to input and output

    while (SDL_PollEvent(&e) != 0)//Handle events on queue
    {
        // If the users post an event to quit
        // An example is hitting the "x" in the corner of the window.
        if (e.type == SDL_QUIT)
        {
            cout << "Goodbye!" << endl;
            gQuit = true;
        }
    }

    // Clear the screen || Changing Window Color
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);// Set the clear color each frame
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);// Clear color buffer and depth buffer

}

void MainLoop()
{

    while (!gQuit)
    {
        Input();//Handle input

        //PreDraw();//Setup anything(i.e. OpenGL State) that needs to take place before draw calls

        //Draw();//Draw Calls in OpenGL

        // Update the screen of specified window
        SDL_GL_SwapWindow(gGraphicsApplicationWindow);
    }
}

void CleanUp()
{
    SDL_DestroyWindow(gGraphicsApplicationWindow);
    SDL_Quit();
}

int main(int argc, char* args[]) {
    //1. Sets ups SDL Window and OpenGL (Graphics Program set up)
    InitalizeProgram();

    //2. Setup our geometry
    //VertexSpecification();

    //3. Create our graphics pipeline
    // - At a minimum, this means the vertex and fragment shader
    //CreateGraphicsPipeline();

    //4. Call the main application loop
    MainLoop();

    //5. Call the cleanup function when our program terminates
    CleanUp();

    return 0;
}