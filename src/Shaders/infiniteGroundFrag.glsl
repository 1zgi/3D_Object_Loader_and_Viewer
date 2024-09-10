#version 410 core
out vec4 FragColor;

in vec3 WorldPos;

uniform vec3 cameraPos;

const vec3 gridColor = vec3(0.8, 0.8, 0.8);
const vec3 backgroundColor = vec3(0.9, 0.9, 0.9);

float getGrid(vec2 p, float scale) {
    vec2 g = abs(fract(p - 0.5) - 0.5) / fwidth(p * scale);
    return min(g.x, g.y);
}

void main()
{
    float scale = 1.0; // Adjust this to change grid size
    float d = getGrid(WorldPos.xz, scale);
    
    vec3 color = mix(gridColor, backgroundColor, min(d, 1.0));
    
    // Add some depth fade
    float dist = length(WorldPos.xz - cameraPos.xz);
    float alpha = 1.0 - smoothstep(20.0, 120.0, dist);
    
    FragColor = vec4(color, alpha);
}