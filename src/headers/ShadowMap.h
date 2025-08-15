#pragma once
#ifndef SHADOWMAP_H
#define SHADOWMAP_H
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include "Lights.h"

class ShadowMap {
public:
    // Constructor and Destructor
    ShadowMap(GLsizei width = 1024, GLsizei height = 1024);  // Constructor with default width and height
    ~ShadowMap();

    // Initialize the shadow map (creates FBO and depth texture)
    void init();

    // Bind the framebuffer for the shadow pass (during light rendering)
    void bindForShadowPass() const;

    void bindForCameraView() const;

    // Bind the shadow map for use during the lighting pass
    void bindForLightingPass(GLuint textureUnit) const;

    // Calculate the light-space matrix for the shadow map
    void calculateLightSpaceMatrix(const glm::vec3& lightPos, const glm::vec3& targetPos, LightType lightType);

    // Resize the shadow map (re-allocate the depth texture)
    void resize(GLsizei width, GLsizei height);

    // Getters
    GLuint getDepthMapTexture() const;      // Retrieve the depth texture
    GLuint getFBO() const;                  // Retrieve the framebuffer object
    glm::mat4 getLightSpaceMatrix() const;  // Retrieve the light-space matrix

    // Setters
    void setLightSpaceMatrix(const glm::mat4& matrix)const;

private:
    GLsizei shadowWidth;  // Width of the shadow map
    GLsizei shadowHeight; // Height of the shadow map

    GLuint FBO;           // Framebuffer object
    GLuint depthMap;       // Depth texture for storing shadow map
    glm::mat4 lightSpaceMatrix; // Light-space transformation matrix
};

#endif
