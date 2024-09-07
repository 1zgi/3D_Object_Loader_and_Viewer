#version 410 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexUV;

out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec2 UV;

uniform mat4 MVP; // Model-View-Projection matrix
uniform mat4 V;   // View matrix
uniform mat4 M;   // Model matrix

void main() {
    // Transform the vertex position into clip space
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);

    // Transform the vertex position into world space
    Position_worldspace = vec3(M * vec4(vertexPosition_modelspace, 1.0));

    // Transform the vertex position into camera space
    vec3 vertexPosition_cameraspace = vec3(V * vec4(Position_worldspace, 1.0));

    // Calculate the direction from the vertex to the camera in camera space
    EyeDirection_cameraspace = -vertexPosition_cameraspace;

    // Transform the normal vector into camera space
    Normal_cameraspace = normalize(mat3(V * M) * vertexNormal_modelspace);

    // Pass UV to the fragment shader
    UV = vertexUV;
}
