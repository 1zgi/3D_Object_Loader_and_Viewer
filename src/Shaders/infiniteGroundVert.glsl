#version 410 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 WorldPos;

void main()
{
    WorldPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(WorldPos, 1.0);
}