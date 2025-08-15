#pragma once
#ifndef INFINITEGROUND_H
#define INFINITEGROUND_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "Lights.h"
#include "ShadowMap.h"
#include <vector>


class InfiniteGround {
public:
    InfiniteGround();
    ~InfiniteGround();

    // Set the height of the ground
    void setHeight(float height);

    // Render the ground
    void renderGround(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection,
        std::vector<Lights>& directionalLights,
        std::vector<Lights>& pointLights,
        std::vector<Lights>& spotLights,
        glm::vec3 backgroundcolor, ShadowMap& shadowMap, bool shadowsEnabled = true);

    void initGround(GLuint shaderProgram);

    void setShader(GLuint shaderProgram);

    void DrawGround();

    glm::mat4 getGroundMatrix() const;
    float getHeight() const;


private:
    GLuint VAO, VBO, EBO;  // OpenGL buffers for the ground
    float groundHeight;    // Ground's vertical position (Y-coordinate)

    GLuint GroundShaderID;

    void renderGroundLights(std::vector<Lights>& directionalLights, std::vector<Lights>& pointLights, std::vector<Lights>& spotLights);
    void setupBuffers();
    glm::mat4 calculateGroundMatrix() const;
    glm::mat4 calculateMVP(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    void sendMatrixToShader(GLuint shaderProgram, glm::mat4 MVP, const glm::mat4& view);

    glm::mat4 modelMatrix; // Model matrix for the ground
};
#endif // INFINITEGROUND_H
