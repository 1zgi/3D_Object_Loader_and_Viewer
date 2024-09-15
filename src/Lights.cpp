#include "headers/Lights.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Lights::Lights(LightType type)
    : type(type), position(0.0f), direction(0.0f, -1.0f, 0.0f), intensity(1.0f),
    ambientIntensity(0.2f), specularIntensity(1.0f),
    constantAttenuation(1.0f), linearAttenuation(0.09f), quadraticAttenuation(0.032f),
    cutOff(glm::cos(glm::radians(12.5f))), outerCutOff(glm::cos(glm::radians(17.5f))) {}

void Lights::setPosition(const glm::vec3& pos) {
    position = pos;
}

void Lights::setDirection(const glm::vec3& dir) {
    direction = dir;
}

void Lights::setIntensity(const glm::vec3& intens) {
    intensity = intens;
}

void Lights::setAmbientIntensity(const glm::vec3& ambientIntens) {
    ambientIntensity = ambientIntens;
}

void Lights::setSpecularIntensity(const glm::vec3& specularIntens) {
    specularIntensity = specularIntens;
}

void Lights::setAttenuation(float constant, float linear, float quadratic) {
    constantAttenuation = constant;
    linearAttenuation = linear;
    quadraticAttenuation = quadratic;
}

void Lights::setCutOff(float c) {
    cutOff = glm::cos(glm::radians(c));
}

void Lights::setOuterCutOff(float outerC) {
    outerCutOff = glm::cos(glm::radians(outerC));
}

void Lights::sendToShader(GLuint programID, const std::string& lightName) {
    std::string lightPrefix = lightName + ".";

    // Ensure the shader program is active
    glUseProgram(programID);

    if (type == LightType::DIRECTIONAL) {
        // Send directional light uniforms
        glUniform3fv(glGetUniformLocation(programID, (lightPrefix + "Direction").c_str()), 1, glm::value_ptr(direction));
        glUniform3fv(glGetUniformLocation(programID, (lightPrefix + "Intensity").c_str()), 1, glm::value_ptr(intensity));
        glUniform3fv(glGetUniformLocation(programID, (lightPrefix + "Ambient").c_str()), 1, glm::value_ptr(ambientIntensity));
        glUniform3fv(glGetUniformLocation(programID, (lightPrefix + "Specular").c_str()), 1, glm::value_ptr(specularIntensity));
    }
    else if (type == LightType::POINT) {
        // Send point light uniforms
        glUniform3fv(glGetUniformLocation(programID, (lightPrefix + "Position").c_str()), 1, glm::value_ptr(position));
        glUniform3fv(glGetUniformLocation(programID, (lightPrefix + "Intensity").c_str()), 1, glm::value_ptr(intensity));
        glUniform3fv(glGetUniformLocation(programID, (lightPrefix + "Ambient").c_str()), 1, glm::value_ptr(ambientIntensity));
        glUniform3fv(glGetUniformLocation(programID, (lightPrefix + "Specular").c_str()), 1, glm::value_ptr(specularIntensity));
        glUniform1f(glGetUniformLocation(programID, (lightPrefix + "Constant").c_str()), constantAttenuation);
        glUniform1f(glGetUniformLocation(programID, (lightPrefix + "Linear").c_str()), linearAttenuation);
        glUniform1f(glGetUniformLocation(programID, (lightPrefix + "Quadratic").c_str()), quadraticAttenuation);
    }
    else if (type == LightType::SPOT) {
        // Send spot light uniforms
        glUniform3fv(glGetUniformLocation(programID, (lightPrefix + "Position").c_str()), 1, glm::value_ptr(position));
        glUniform3fv(glGetUniformLocation(programID, (lightPrefix + "Direction").c_str()), 1, glm::value_ptr(direction));
        glUniform3fv(glGetUniformLocation(programID, (lightPrefix + "Intensity").c_str()), 1, glm::value_ptr(intensity));
        glUniform3fv(glGetUniformLocation(programID, (lightPrefix + "Ambient").c_str()), 1, glm::value_ptr(ambientIntensity));
        glUniform3fv(glGetUniformLocation(programID, (lightPrefix + "Specular").c_str()), 1, glm::value_ptr(specularIntensity));
        glUniform1f(glGetUniformLocation(programID, (lightPrefix + "Constant").c_str()), constantAttenuation);
        glUniform1f(glGetUniformLocation(programID, (lightPrefix + "Linear").c_str()), linearAttenuation);
        glUniform1f(glGetUniformLocation(programID, (lightPrefix + "Quadratic").c_str()), quadraticAttenuation);
        glUniform1f(glGetUniformLocation(programID, (lightPrefix + "CutOff").c_str()), cutOff);
        glUniform1f(glGetUniformLocation(programID, (lightPrefix + "OuterCutOff").c_str()), outerCutOff);
    }
}

void Lights::enableDirectionalLights(GLuint programID,int i)
{
    glUniform1i(glGetUniformLocation(programID, ("useDirectionalLight[" + std::to_string(i) + "]").c_str()), true);
}

void Lights::enableSpotLights(GLuint programID, int i)
{
    glUniform1i(glGetUniformLocation(programID, ("useSpotLight[" + std::to_string(i) + "]").c_str()), true);
}

void Lights::enablePointLights(GLuint programID, int i)
{
    glUniform1i(glGetUniformLocation(programID, ("usePointLight[" + std::to_string(i) + "]").c_str()), true);
}

glm::vec3 Lights::getPosition() const {
    return position;
}

glm::vec3 Lights::getDirection() const {
    return direction;
}

glm::vec3 Lights::getIntensity() const {
    return intensity;
}

glm::vec3 Lights::getAmbientIntensity() const {
    return ambientIntensity;
}

glm::vec3 Lights::getSpecularIntensity() const {
    return specularIntensity;
}

// Getter for Constant Attenuation
float Lights::getConstant() const {
    return constantAttenuation;
}

// Getter for Linear Attenuation
float Lights::getLinear() const {
    return linearAttenuation;
}

// Getter for Quadratic Attenuation
float Lights::getQuadratic() const {
    return quadraticAttenuation;
}

// Getter for Spot Light Cut-Off Angle
float Lights::getCutOff() const {
    return cutOff;
}

// Getter for Spot Light Outer Cut-Off Angle
float Lights::getOuterCutOff() const {
    return outerCutOff;
}