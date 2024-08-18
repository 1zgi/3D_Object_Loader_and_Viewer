#version 330 core

in vec3 fragmentColor;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

uniform vec3 AmbientLightIntensity;

out vec4 color;

void main() {
    vec3 MaterialDiffuseColor = fragmentColor;
    vec3 MaterialAmbientColor = AmbientLightIntensity * MaterialDiffuseColor;
    vec3 MaterialSpecularColor = vec3(0.3, 0.3, 0.3);

    // Ensure normals are normalized before use
    vec3 n = normalize(Normal_cameraspace);
    vec3 l = normalize(LightDirection_cameraspace);
    
    // Diffuse lighting
    float cosTheta = max(dot(n, l), 0.0);  // Changed clamp to max for better readability

    // Specular lighting
    vec3 E = normalize(EyeDirection_cameraspace);
    vec3 R = reflect(-l, n);
    float cosAlpha = max(dot(E, R), 0.0);  // Changed clamp to max for better readability

    // Reduce the specular power to soften the highlight, making lighting more uniform
    vec3 finalColor = MaterialAmbientColor + 
                      MaterialDiffuseColor * cosTheta + 
                      MaterialSpecularColor * pow(cosAlpha, 3.0);  // Reduced specular power

    color = vec4(finalColor, 1.0);
}
