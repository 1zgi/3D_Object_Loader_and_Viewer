#pragma once
#ifndef INFINITEGROUND_H
#define INFINITEGROUND_H

#include <GL/glew.h>
#include <glm/glm.hpp>

class InfiniteGround {
public:
    InfiniteGround();
    ~InfiniteGround();

    // Set the height of the ground
    void setHeight(float height);

    // Render the ground
    void renderGround(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection);

private:
    GLuint VAO, VBO, EBO;  // OpenGL buffers for the ground
    float groundHeight;    // Ground's vertical position (Y-coordinate)

    glm::mat4 modelMatrix; // Model matrix for the ground
};

#endif // INFINITEGROUND_H
