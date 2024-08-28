#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "stb_image.h"
#include "tiny_obj_loader.h"
#include "shader.hpp"

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

    GLuint getTextureID(size_t materialIndex) const;
    GLuint getSpecularTextureID(size_t materialIndex) const;

private:
    // Loads the model from an OBJ file
    void loadModel(const std::string& filepath);

    // Loads textures associated with the model
    void loadTextures();

    // Sets up OpenGL buffers (VAO, VBO, NBO, TBO, EBO)
    void setupBuffers();

    // Calculates the model transformation matrix based on the position, rotation, and scale
    glm::mat4 calculateModelMatrix() const;

    // The model's vertices, normals, texture coordinates, and indices
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texcoords;
    std::vector<unsigned int> indices;

    // Material information
    std::vector<tinyobj::material_t> materials;
    std::vector<int> face_material_ids; // Stores material ID for each face

    // OpenGL handles for the model's buffers
    GLuint vao;
    GLuint vbo;
    GLuint nbo;
    GLuint tbo;
    GLuint ebo;

    // OpenGL handles for textures
    std::vector<GLuint> textures;
    std::vector<GLuint> specularTextures;

    // Model transformation attributes
    glm::vec3 position;
    glm::vec3 rotationAxis;
    float rotationAngle;
    glm::vec3 scale;
};

#endif // MODEL_H
