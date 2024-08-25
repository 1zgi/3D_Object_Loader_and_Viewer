#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

out vec3 fragmentColor;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

uniform mat4 MVP; // Model-View-Projection matrix
uniform mat4 V;   // View matrix
uniform mat4 M;   // Model matrix
uniform vec3 LightPosition_worldspace; // Light position in world space

void main() {
    // Transform the vertex position into clip space
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);

    // Transform the vertex position into world space
    Position_worldspace = vec3(M * vec4(vertexPosition_modelspace, 1.0));

    // Transform the vertex position into camera space
    vec3 vertexPosition_cameraspace = vec3(V * vec4(Position_worldspace, 1.0));

    // Calculate the direction from the vertex to the camera in camera space
    EyeDirection_cameraspace = -vertexPosition_cameraspace;

    // Calculate the direction from the vertex to the light in camera space
    LightDirection_cameraspace = vec3(V * vec4(LightPosition_worldspace, 1.0)) - vertexPosition_cameraspace;

    // Transform the normal vector into camera space
    Normal_cameraspace = normalize(mat3(V) * mat3(M) * vertexNormal_modelspace);

    // Set a default color (for example, white) to pass to the fragment shader
    fragmentColor = vec3(1.0, 1.0, 1.0);
}
