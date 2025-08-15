#include "headers/ShadowMap.h"

// Constructor
ShadowMap::ShadowMap(GLsizei width, GLsizei height)
    : shadowWidth(width), shadowHeight(height), FBO(0), depthMap(0), lightSpaceMatrix(glm::mat4(1.0f)) {}

// Destructor
ShadowMap::~ShadowMap() {
    if (FBO) {
        glDeleteFramebuffers(1, &FBO);
    }
    if (depthMap) {
        glDeleteTextures(1, &depthMap);
    }
}

// Initialize the shadow map (FBO and depth texture)
void ShadowMap::init() {
    // Generate framebuffer
    glGenFramebuffers(1, &FBO);

    // Generate depth texture for shadow map
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // Set border color (usually white for shadow maps)
    GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach depth texture to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

    // We don't need a color buffer for shadow mapping
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Check if the framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Shadow map framebuffer is not complete!" << std::endl;
    }

    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Bind for shadow pass (rendering the scene from the light's perspective)
void ShadowMap::bindForShadowPass() const {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, shadowWidth, shadowHeight);  // Set viewport to shadow map size
    glClear(GL_DEPTH_BUFFER_BIT);  // Clear the depth buffer
}

void ShadowMap::bindForCameraView() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1024, 768);  // Set viewport to screen size
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the depth buffer
}

// Bind the shadow map for the lighting pass (use shadow map in shaders)
void ShadowMap::bindForLightingPass(GLuint textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, depthMap);
}

// Calculate the light-space matrix (transform world coordinates to light space)
void ShadowMap::calculateLightSpaceMatrix(const glm::vec3& lightPos, const glm::vec3& targetPos, LightType lightType) {
    glm::mat4 lightProjection, lightView;

    if (lightType == LightType::DIRECTIONAL) {
        // Orthographic projection for directional light (smaller bounds for tighter shadows)
        float nearPlane = 1.0f, farPlane = 50.0f;
        lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, nearPlane, farPlane);
        lightView = glm::lookAt(lightPos, targetPos, glm::vec3(0.0f, 1.0f, 0.0f));  // View from light's perspective
    }
    else if (lightType == LightType::SPOT || lightType == LightType::POINT) {
        lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 100.0f);
        lightView = glm::lookAt(lightPos, targetPos, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // Store the light space matrix without bias (bias will be applied in the fragment shader)
    lightSpaceMatrix = lightProjection * lightView;
}


// Resize the shadow map (reallocate texture and framebuffer)
void ShadowMap::resize(GLsizei width, GLsizei height) {
    shadowWidth = width;
    shadowHeight = height;

    // Delete the previous framebuffer and texture
    if (FBO) {
        glDeleteFramebuffers(1, &FBO);
    }
    if (depthMap) {
        glDeleteTextures(1, &depthMap);
    }

    // Reinitialize with the new dimensions
    init();
}

// Getters
GLuint ShadowMap::getDepthMapTexture() const {
    return depthMap;
}

GLuint ShadowMap::getFBO() const {
    return FBO;
}

glm::mat4 ShadowMap::getLightSpaceMatrix() const {
    return lightSpaceMatrix;
}

// Setters
void ShadowMap::setLightSpaceMatrix(const glm::mat4& matrix) const{
    const_cast<ShadowMap*>(this)->lightSpaceMatrix = matrix;
}
