#version 410 core

layout (location = 0) out vec4 color;

uniform sampler2D shadow; //depth map

in vec4 lightView_Position;

void main()
{
    color = vec4(1.0, 0.0, 0.0, 1.0);

    vec3 projCoords = lightView_Position.xyz / lightView_Position.w;

    color *=texture(shadow, projCoords.xy).r < projCoords.z ? 0 : 1;
}