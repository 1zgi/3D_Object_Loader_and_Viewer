#version 330 core

in vec3 fragmentColor;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

uniform vec3 AmbientLightIntensity;

out vec4 color;

void main() {
    // Material properties
    vec3 MaterialDiffuseColor = fragmentColor; // Color passed from the vertex shader
    vec3 MaterialAmbientColor = AmbientLightIntensity * MaterialDiffuseColor; // Ambient light contribution
    vec3 MaterialSpecularColor = vec3(0.3, 0.3, 0.3); // Specular highlight color

    // Normalize vectors before using them
    vec3 n = normalize(Normal_cameraspace);
    vec3 l = normalize(LightDirection_cameraspace);

    // Calculate diffuse lighting (Lambertian reflectance)
    float cosTheta = max(dot(n, l), 0.0);

    // Calculate specular lighting (Phong reflection model)
    vec3 E = normalize(EyeDirection_cameraspace);
    vec3 R = reflect(-l, n); // Reflect the light direction around the normal
    float cosAlpha = max(dot(E, R), 0.0);

    // Calculate the final color with ambient, diffuse, and specular contributions
    vec3 finalColor = MaterialAmbientColor + 
                      MaterialDiffuseColor * cosTheta + 
                      MaterialSpecularColor * pow(cosAlpha, 32.0); // Specular exponent is 32.0 for a sharper highlight

    // Output the final color
    color = vec4(finalColor, 1.0);
}
