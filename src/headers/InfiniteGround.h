#pragma once
#ifndef INFINITE_GROUND_H
#define INFINITE_GROUND_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class InfiniteGround {
public:
    InfiniteGround();
    ~InfiniteGround();
    void renderGround(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection) const;

private:
    GLuint VAO, VBO, EBO;
};

#endif // INFINITE_GROUND_H