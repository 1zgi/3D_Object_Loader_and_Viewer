#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec3 fragmentColor;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec3 AmbientLightColor;  // New output to pass ambient light to fragment shader

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace;
uniform vec3 AmbientLightIntensity;  // New uniform for ambient light

void main() {
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);

    // Compute various space transformations
    Position_worldspace = (M * vec4(vertexPosition_modelspace, 1.0)).xyz;
    vec3 vertexPosition_cameraspace = (V * M * vec4(vertexPosition_modelspace, 1.0)).xyz;
    EyeDirection_cameraspace = vec3(0.0, 0.0, 0.0) - vertexPosition_cameraspace;
    LightDirection_cameraspace = (V * vec4(LightPosition_worldspace, 1.0)).xyz - vertexPosition_cameraspace;
    Normal_cameraspace = (V * M * vec4(vertexNormal_modelspace, 0.0)).xyz;

    // Pass through the vertex color
    fragmentColor = vertexColor;

    // Pass the ambient light intensity to the fragment shader
    AmbientLightColor = AmbientLightIntensity;
}
