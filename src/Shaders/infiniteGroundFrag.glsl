#version 410 core
out vec4 FragColor;

uniform vec3 backgroundColor;  // The background color passed from the program

void main()
{
    // Slightly darken or lighten the background color for the ground
    vec3 groundColor = backgroundColor * 0.95;  // 5% darker than the background
    FragColor = vec4(groundColor, 1.0);  // Fully opaque
}
