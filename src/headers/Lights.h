#pragma once
#ifndef LIGHTS_H
#define LIGHTS_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>

// Types of lights
enum class LightType {
    DIRECTIONAL,
    POINT,
    SPOT
};

class Lights {
public:
    Lights(LightType type);

    // Set light properties
    void setPosition(const glm::vec3& position);
    void setDirection(const glm::vec3& direction);
    void setIntensity(const glm::vec3& intensity);
    void setAmbientIntensity(const glm::vec3& ambientIntensity);
    void setSpecularIntensity(const glm::vec3& specularIntensity);
    void setAttenuation(float constant, float linear, float quadratic);

    // **Getters** for each light property
    glm::vec3 getPosition() const;
    glm::vec3 getDirection() const;
    glm::vec3 getIntensity() const;
    glm::vec3 getAmbientIntensity() const;
    glm::vec3 getSpecularIntensity() const;
    float getConstant() const;
    float getLinear() const;
    float getQuadratic() const;
    float getCutOff() const;
    float getOuterCutOff() const;

    // For spotlights
    void setCutOff(float cutOff);
    void setOuterCutOff(float outerCutOff);

    // Send the light properties to the shader
    void sendToShader(GLuint programID, const std::string& lightName);

private:
    LightType type;

    // Common light properties
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 intensity;
    glm::vec3 ambientIntensity;
    glm::vec3 specularIntensity;

    // Attenuation for point/spot lights
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;

    // SpotLight-specific properties
    float cutOff;
    float outerCutOff;
};

#endif
