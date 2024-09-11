#include "headers/InfiniteGround.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <iostream>  // For debugging

InfiniteGround::InfiniteGround()
    : groundHeight(0.0f), modelMatrix(glm::mat4(1.0f))  // Initialize ground height and model matrix
{
    // Define vertices for a large ground plane
    float quadVertices[] = {
        -500.0f,  0.0f, -500.0f,   // Bottom-left
         500.0f,  0.0f, -500.0f,   // Bottom-right
         500.0f,  0.0f,  500.0f,   // Top-right
        -500.0f,  0.0f,  500.0f    // Top-left
    };

    // Reverse the winding order to flip normals
    unsigned int quadIndices[] = {
        0, 3, 2,   // Triangle 1
        2, 1, 0    // Triangle 2
    };

    // Generate and bind VAO, VBO, EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);  // Unbind VAO
}

InfiniteGround::~InfiniteGround() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void InfiniteGround::renderGround(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection) {
    glm::mat4 MVP = projection * view * modelMatrix;

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void InfiniteGround::setHeight(float height) {
    groundHeight = height;

    float groundOffset = 0.001f;
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, groundHeight - groundOffset, 0.1f));
}
