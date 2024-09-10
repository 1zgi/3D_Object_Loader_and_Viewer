#include "headers/InfiniteGround.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

InfiniteGround::InfiniteGround() {
    // Define vertices for a simple plane
    float quadVertices[] = {
        -1.0f,  0.0f, -1.0f,   // Bottom-left
         1.0f,  0.0f, -1.0f,   // Bottom-right
         1.0f,  0.0f,  1.0f,   // Top-right
        -1.0f,  0.0f,  1.0f    // Top-left
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

    glBindVertexArray(0);
}

InfiniteGround::~InfiniteGround() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void InfiniteGround::renderGround(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection) const {
    glUseProgram(shaderProgram);

    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    if (viewLoc != -1 && projectionLoc != -1) {
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));  // Move ground slightly downwards
    model = glm::scale(model, glm::vec3(1000.0f, 1.0f, 1000.0f));  // Large scale for "infinite" effect

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    if (modelLoc != -1) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
