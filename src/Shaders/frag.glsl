#version 410 core

// Define the maximum number of directional and spotlights
#define MAX_LIGHTS 4

in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec2 UV;

struct Material {
    vec3 AmbientColor;
    vec3 DiffuseColor;
    vec3 SpecularColor;
    float Shininess;
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
    vec3 Ambient;
    float CutOff;
    float OuterCutOff;
    float Constant;
    float Linear;
    float Quadratic;
};

uniform Material material;
uniform DirectionalLight dirLights[MAX_LIGHTS]; // Array for multiple directional lights
uniform SpotLight spotLights[MAX_LIGHTS];       // Array for multiple spotlights
uniform int numDirLights;                       // Number of active directional lights
uniform int numSpotLights;                      // Number of active spotlights

uniform vec3 AmbientLightIntensity;

uniform bool useSpotLight[MAX_LIGHTS];           // Toggle for each spotlight
uniform bool useDirectionalLight[MAX_LIGHTS];    // Toggle for each directional light
uniform bool useTexture;                         // Check if we should use the texture

out vec4 color;

// Function to calculate directional light contribution
vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 MaterialDiffuseColor) {
    vec3 lightDir = normalize(-light.Direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.Intensity * diff * MaterialDiffuseColor;

    // Specular shading (Phong reflection model)
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);
    vec3 specular = light.Intensity * spec * material.SpecularColor;

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

    // Ambient shading
    vec3 ambient = light.Ambient * MaterialDiffuseColor;

    // Attenuation based on distance
    float distance = length(light.Position - fragPos);
    float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * (distance * distance));

    // Diffuse lighting
    float cosTheta = max(dot(normal, toLight), 0.0);
    vec3 diffuse = light.Intensity * MaterialDiffuseColor * cosTheta * intensity * attenuation;

    // Specular reflection (Phong reflection model)
    vec3 reflectDir = reflect(-toLight, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);
    vec3 specular = light.Intensity * spec * material.SpecularColor * intensity * attenuation;

    // Combine diffuse and specular
    return ambient + diffuse + specular;
}

void main() {
    vec3 MaterialDiffuseColor;

    // Normalize the normal vector
    vec3 normal = normalize(Normal_cameraspace);

    // Camera (view) direction
    vec3 viewDir = normalize(EyeDirection_cameraspace);

    // Initialize the final color
    vec3 finalColor = vec3(0.0);

    // Use the diffuse texture if available, otherwise fallback to the diffuse color
    if (useTexture) {
        MaterialDiffuseColor = texture(material.diffuseTexture, UV).rgb;
        MaterialDiffuseColor = pow(MaterialDiffuseColor, vec3(2.2));  // Gamma correction for sRGB textures
    } else {
        MaterialDiffuseColor = material.DiffuseColor;  // Fallback to diffuse color
    }

    // Apply directional lights if enabled
    for (int i = 0; i < numDirLights; ++i) {
        if (useDirectionalLight[i]) {
            finalColor += calcDirLight(dirLights[i], normal, viewDir, MaterialDiffuseColor);
        }
    }

    // Apply spotlights if enabled
    for (int i = 0; i < numSpotLights; ++i) {
        if (useSpotLight[i]) {
            finalColor += calcSpotLight(spotLights[i], normal, viewDir, Position_worldspace, MaterialDiffuseColor);
        }
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
