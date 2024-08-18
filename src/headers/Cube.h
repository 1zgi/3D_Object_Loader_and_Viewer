#ifndef CUBE_H
#define CUBE_H

#include <GL/glew.h>

class Cube {
public:
    Cube();
    ~Cube();
    void init();
    void draw();
private:
    GLuint vertexArrayID;
    GLuint vertexbuffer;
    GLuint colorbuffer;
    GLuint normalbuffer;
};

#endif
