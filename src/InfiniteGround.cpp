#include "headers/InfiniteGround.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <iostream>

// Vertices (position + normal) for a simple quad ground plane
float quadVertices[] = {
    // Positions           // Normals
    -500.0f,  0.0f, -500.0f,   0.0f, 1.0f, 0.0f,  // Bottom-left
     500.0f,  0.0f, -500.0f,   0.0f, 1.0f, 0.0f,  // Bottom-right
     500.0f,  0.0f,  500.0f,   0.0f, 1.0f, 0.0f,  // Top-right
    -500.0f,  0.0f,  500.0f,   0.0f, 1.0f, 0.0f   // Top-left
};

// Reverse the winding order to flip normals
unsigned int quadIndices[] = {
    0, 3, 2,   // Triangle 1
    2, 1, 0    // Triangle 2
};

InfiniteGround::InfiniteGround()
    : groundHeight(0.0f), modelMatrix(glm::mat4(1.0f)), GroundShaderID(0),VAO(0),VBO(0) ,EBO(0){}  // Initialize ground height and model matrix

InfiniteGround::~InfiniteGround() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void InfiniteGround::initGround(GLuint shaderProgram) {
    setShader(shaderProgram);
    setupBuffers();
}

void InfiniteGround::renderGround(const glm::mat4& view, const glm::mat4& projection,
    std::vector<Lights>& directionalLights,
    std::vector<Lights>& pointLights,
    std::vector<Lights>& spotLights,
    glm::vec3 backgroundcolor) 
{
    GLuint backgroundColorLocation = glGetUniformLocation(GroundShaderID, "backgroundColor");
    glUniform3fv(backgroundColorLocation, 1, &backgroundcolor[0]);

    // Use the shader program for the ground
    glUseProgram(GroundShaderID);

    modelMatrix = getGroundMatrix();

    // Calculate the Model-View-Projection (MVP) matrix
    glm::mat4 MVP = projection * view * modelMatrix;

    // Send the MVP matrix to the shader
    glUniformMatrix4fv(glGetUniformLocation(GroundShaderID, "MVP"), 1, GL_FALSE, &MVP[0][0]);

    // Send the model matrix to the shader (for lighting calculations)
    glUniformMatrix4fv(glGetUniformLocation(GroundShaderID, "M"), 1, GL_FALSE, &modelMatrix[0][0]);

    // Send the view matrix to the shader
    glUniformMatrix4fv(glGetUniformLocation(GroundShaderID, "V"), 1, GL_FALSE, &view[0][0]);

    renderGroundLights(directionalLights, pointLights, spotLights);

    // Material color
    glm::vec3 materialDiffuseColor = glm::vec3(0.8f, 0.8f, 0.8f);  // Light gray diffuse
    glm::vec3 materialSpecularColor = glm::vec3(1.0f, 1.0f, 1.0f);  // White specular
    float materialShininess = 35.0f;  // Shiny material

    glUniform3fv(glGetUniformLocation(GroundShaderID, "materialDiffuseColor"), 1, &backgroundcolor[0]);
    glUniform3fv(glGetUniformLocation(GroundShaderID, "materialSpecularColor"), 1, &materialSpecularColor[0]);
    glUniform1f(glGetUniformLocation(GroundShaderID, "materialShininess"), materialShininess);

    DrawGround();
}

void InfiniteGround::renderGroundLights(std::vector<Lights>& directionalLights, std::vector<Lights>& pointLights, std::vector<Lights>& spotLights)
{
    // Send the number of directional and spotlights to the shader
    glUniform1i(glGetUniformLocation(GroundShaderID, "numDirLights"), static_cast<GLint>(directionalLights.size()));
    glUniform1i(glGetUniformLocation(GroundShaderID, "numSpotLights"), static_cast<GLint>(spotLights.size()));

    // Pass all directional lights to the shader
    for (int i = 0; i < static_cast<GLint>(directionalLights.size()); ++i) {
        directionalLights[i].sendToShader(GroundShaderID, "dirLights[" + std::to_string(i) + "]");

        directionalLights[i].enableDirectionalLights(GroundShaderID, i);
    }

    // Pass all spotlights to the shader
    for (int i = 0; i < static_cast<GLint>(spotLights.size()); ++i) {
        spotLights[i].sendToShader(GroundShaderID, "spotLights[" + std::to_string(i) + "]");

        spotLights[i].enableSpotLights(GroundShaderID, i);
    }

    // Pass all point lights to the shader
    for (int i = 0; i < pointLights.size(); ++i) {
        pointLights[i].sendToShader(GroundShaderID, "pointLights[" + std::to_string(i) + "]");

        //Enable point light
    }
}

void InfiniteGround::DrawGround()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // Unbind the shader program
    glUseProgram(0);
}

void InfiniteGround::setupBuffers() {
    // Generate and bind VAO, VBO, EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Bind and set VBO for position and normal data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Bind and set EBO for indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // Enable vertex attribute for position (location = 0 in shader)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Enable vertex attribute for normals (location = 1 in shader)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);
}

glm::mat4 InfiniteGround::calculateGroundMatrix() const{
    glm::mat4 groundMatrix = glm::mat4(1.0f);
    groundMatrix = glm::translate(groundMatrix, glm::vec3(0.0f, getHeight(), 0.0f));
    return groundMatrix;
}

glm::mat4 InfiniteGround::getGroundMatrix() const {
    return calculateGroundMatrix();
}

float InfiniteGround::getHeight() const{
    return groundHeight;
}

void InfiniteGround::setHeight(float height) {
    groundHeight = height;

    float groundOffset = 0.001f;
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, groundHeight - groundOffset, 0.1f));
}

void InfiniteGround::setShader(GLuint shaderProgram) {
    GroundShaderID = shaderProgram;
}