#version 410 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

out vec3 FragPos_worldspace;
out vec3 Normal_cameraspace;

uniform mat4 MVP;  // Model-View-Projection matrix
uniform mat4 M;    // Model matrix
uniform mat4 V;    // View matrix

void main() {
    // Calculate world space position
    FragPos_worldspace = vec3(M * vec4(vertexPosition_modelspace, 1.0));

    // Transform the normal to camera space (M is the model matrix)
    Normal_cameraspace = mat3(transpose(inverse(M))) * vertexNormal_modelspace;

    // Calculate the final vertex position
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
}
