#version 410 core

layout (location = 0) in vec3 position;

uniform mat4 lightMVP;
uniform mat4 matrixShadow;


out vec4 lightView_Position;

void main()
{
    gl_Position = lightMVP * vec4(position, 1.0);
    lightView_Position = matrixShadow * vec4(position, 1.0);
}