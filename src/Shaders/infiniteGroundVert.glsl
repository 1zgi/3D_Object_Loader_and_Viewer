#version 410 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

out vec3 FragPos_worldspace;   // World space position for lighting
out vec3 Normal_cameraspace;   // Normal in camera space for lighting
out vec4 FragPosLightSpace;    // Position in light space for shadow mapping

uniform mat4 MVP;              // Model-View-Projection matrix (camera)
uniform mat4 M;                // Model matrix (world transformation)
uniform mat4 V;                // View matrix (camera view)
uniform mat4 lightSpaceMatrix; // Light's view-projection matrix (for shadow mapping)

void main() {
    // Calculate world space position
    FragPos_worldspace = vec3(M * vec4(vertexPosition_modelspace, 1.0));

    // Transform the normal to camera space
    Normal_cameraspace = mat3(transpose(inverse(M))) * vertexNormal_modelspace;

    // Calculate the position in light space for shadow mapping
    FragPosLightSpace = lightSpaceMatrix * M * vec4(vertexPosition_modelspace, 1.0);

    // Final position in camera space
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
}
