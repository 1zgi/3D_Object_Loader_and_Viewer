#pragma once

#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>
#include<iostream>

#include <vector>
#include "VAO.h"
#include "EBO.h"
#include "Camera.h"

#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Mesh
{
public:
    std::vector <Vertex> vertices;//C++ vectors not GLM vectors
    std::vector <GLuint> indices;
    std::vector <Texture> textures;
    // Store VAO in public so it can be used in the Draw function
    VAO VAO;

    // Initializes the mesh
    Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);

    // Draws the mesh
    void Draw
    (
        Shader& shader,
        Camera& camera,
        glm::mat4 matrix = glm::mat4(1.0f),
        glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
    );
};
#endif
