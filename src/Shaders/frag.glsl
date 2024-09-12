#version 410 core

in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec2 UV;

struct Material {
    vec3 AmbientColor;
    vec3 DiffuseColor;
    vec3 SpecularColor;
    sampler2D diffuseTexture;
    sampler2D specularTexture;
};

struct DirectionalLight {
    vec3 Direction;
    vec3 Intensity;  // Light intensity (used for diffuse and specular calculations)
    vec3 Ambient;    // Ambient light color
};

struct SpotLight {
    vec3 Position;
    vec3 Direction;
    vec3 Intensity;
    float CutOff;
    float OuterCutOff;
    float Constant;
    float Linear;
    float Quadratic;
};

uniform Material material;
uniform SpotLight spotLight;
uniform DirectionalLight dirLight;
uniform vec3 AmbientLightIntensity;

uniform bool useSpotLight;           // Toggle for spotlight
uniform bool useDirectionalLight;    // Toggle for directional light
uniform bool useTexture;             // Check if we should use the texture

out vec4 color;

// Function to calculate directional light contribution
vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 MaterialDiffuseColor) {
    vec3 lightDir = normalize(-light.Direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.Intensity * diff * MaterialDiffuseColor;

    // Specular shading (Phong reflection model)
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    vec3 specular = light.Intensity * spec * material.SpecularColor;  // Use light's Intensity for specular calculations

    // Ambient shading
    vec3 ambient = light.Ambient * MaterialDiffuseColor;

    // Return combined result
    return ambient + diffuse + specular;
}

// Function to calculate spotlight contribution
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 MaterialDiffuseColor) {
    vec3 toLight = normalize(light.Position - fragPos);
    float theta = dot(toLight, normalize(-light.Direction));

    // Spotlight intensity based on cutoff angles
    float epsilon = light.CutOff - light.OuterCutOff;
    float intensity = clamp((theta - light.OuterCutOff) / epsilon, 0.0, 1.0);

    // Attenuation based on distance
    float distance = length(light.Position - fragPos);
    float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * (distance * distance));

    // Diffuse lighting
    float cosTheta = max(dot(normal, toLight), 0.0);
    vec3 diffuse = light.Intensity * MaterialDiffuseColor * cosTheta * intensity * attenuation;

    // Specular reflection (Phong reflection model)
    vec3 reflectDir = reflect(-toLight, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    vec3 specular = light.Intensity * spec * material.SpecularColor * intensity * attenuation;

    // Combine diffuse and specular
    return diffuse + specular;
}

void main() {
    vec3 MaterialDiffuseColor;

    // Use the diffuse texture if available, otherwise fallback to the diffuse color
    if (useTexture) {
        MaterialDiffuseColor = texture(material.diffuseTexture, UV).rgb;
        MaterialDiffuseColor = pow(MaterialDiffuseColor, vec3(2.2));  // Gamma correction for sRGB textures
    } else {
        MaterialDiffuseColor = material.DiffuseColor;  // Fallback to diffuse color
    }

    // Normalize the normal vector
    vec3 normal = normalize(Normal_cameraspace);

    // Camera (view) direction
    vec3 viewDir = normalize(EyeDirection_cameraspace);

    // Initialize the final color
    vec3 finalColor = vec3(0.0);

    // Apply directional light if enabled
    if (useDirectionalLight) {
        finalColor += calcDirLight(dirLight, normal, viewDir, MaterialDiffuseColor);
    }

    // Apply spotlight if enabled
    if (useSpotLight) {
        finalColor += calcSpotLight(spotLight, normal, viewDir, Position_worldspace, MaterialDiffuseColor);
    }

    // Ambient lighting from the global ambient intensity
    vec3 MaterialAmbientColor = AmbientLightIntensity * MaterialDiffuseColor;

    // Combine with ambient light
    finalColor += MaterialAmbientColor;

    // Gamma correction
    finalColor = pow(finalColor, vec3(1.0 / 2.2));

    // Output the final color
    color = vec4(finalColor, 1.0);
}
