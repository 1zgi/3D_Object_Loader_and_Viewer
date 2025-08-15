#version 410 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexUV;

out vec3 Position_worldspace;      // World space position for lighting
out vec3 Normal_cameraspace;       // Normal in camera space for lighting
out vec3 EyeDirection_cameraspace; // Direction to the camera in camera space
out vec2 UV;                       // Texture coordinates
out vec4 FragPosLightSpace;        // Position in light space for shadow mapping

uniform mat4 MVP;                  // Model-View-Projection matrix (camera space)
uniform mat4 V;                    // View matrix (camera view)
uniform mat4 M;                    // Model matrix (world transformation)
uniform mat4 lightSpaceMatrix;     // Light's view-projection matrix (for shadow mapping)

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

    // Calculate the position in light space for shadow mapping
    FragPosLightSpace = lightSpaceMatrix * M * vec4(vertexPosition_modelspace, 1.0);

    // Pass UV to the fragment shader
    UV = vertexUV;
}
