#pragma once
#ifndef INFINITEGROUND_H
#define INFINITEGROUND_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Lights.h"


class InfiniteGround {
public:
    InfiniteGround();
    ~InfiniteGround();

    // Set the height of the ground
    void setHeight(float height);

    // Render the ground
    void renderGround(const glm::mat4& view, const glm::mat4& projection, Lights& directionalLight, Lights& pointLight, Lights& spotLight, glm::vec3 backgroundcolor);
    void initGround(GLuint shaderProgram);

    void setShader(GLuint shaderProgram);

    void DrawGround();

    glm::mat4 getGroundMatrix() const;
    float getHeight() const;


private:
    GLuint VAO, VBO, EBO;  // OpenGL buffers for the ground
    float groundHeight;    // Ground's vertical position (Y-coordinate)

    GLuint GroundShaderID;

    void setupBuffers();
    glm::mat4 calculateGroundMatrix() const;

    glm::mat4 modelMatrix; // Model matrix for the ground
};
#endif // INFINITEGROUND_H
