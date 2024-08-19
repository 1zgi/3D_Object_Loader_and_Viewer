#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "tiny_obj_loader.h"

class Model {
public:
    // Constructor: Loads a model from the given OBJ file
    Model(const std::string& filepath);

    // Destructor: Cleans up allocated resources
    ~Model();

    // Renders the model
    void draw() const;

    // Returns the model's transformation matrix
    glm::mat4 getModelMatrix() const;

    // Sets the model's position
    void setPosition(const glm::vec3& position);

    // Sets the model's rotation (in degrees)
    void setRotation(float angle, const glm::vec3& axis);

    // Sets the model's scale
    void setScale(const glm::vec3& scale);

private:
    // Loads the model from an OBJ file
    void loadModel(const std::string& filepath);

    // Sets up OpenGL buffers (VAO, VBO, NBO, TBO)
    void setupBuffers();

    // Calculates the model transformation matrix based on the position, rotation, and scale
    glm::mat4 calculateModelMatrix() const;

    // The model's vertices, normals, and texture coordinates
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texcoords;

    // OpenGL handles for the model's buffers
    GLuint vao;
    GLuint vbo;
    GLuint nbo;
    GLuint tbo;

    // Model transformation attributes
    glm::vec3 position;
    glm::vec3 rotationAxis;
    float rotationAngle;
    glm::vec3 scale;
};

#endif // MODEL_H
