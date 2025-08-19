#version 410 core

// Define the maximum number of directional and spotlights
#define MAX_LIGHTS 4

in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec2 UV;
in vec4 FragPosLightSpace;  // Position in light space for shadow mapping

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

uniform sampler2D shadowMap;   // Shadow map texture
uniform float shadowBias;      // Bias to avoid shadow acne
uniform bool shadowsEnabled;   // Flag to enable/disable shadows

out vec4 color;

// Function to calculate shadow factor with PCF (Percentage Closer Filtering)
float calculateShadow(vec4 fragPosLightSpace) {
    // If shadows are disabled, return no shadow
    if (!shadowsEnabled) {
        return 0.0;
    }

    // Perform perspective division (from clip space to NDC)
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0, 1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Get the current fragment depth from the light's perspective
    float currentDepth = projCoords.z;

    // If the fragment is outside the light's frustum, return no shadow
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.y < 0.0 || projCoords.x > 1.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    // Calculate bias based on surface normal and light direction
    // For directional light, assume light direction is the first directional light
    vec3 lightDir = normalize(-dirLights[0].Direction);
    vec3 normal = normalize(Normal_cameraspace);
    
    // Calculate bias to prevent shadow acne - higher bias for surfaces facing away from light
    float bias = max(shadowBias * (1.0 - dot(normal, lightDir)), shadowBias * 0.1);
    
    // PCF (Percentage Closer Filtering) for soft shadows
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    // Sample a 3x3 grid around the current texel
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }    
    }
    shadow /= 9.0; // Average the 9 samples
    
    // Additional bias for surfaces nearly parallel to light (prevent self-shadowing)
    float surfaceAngle = dot(normal, lightDir);
    if (surfaceAngle < 0.1) {
        shadow *= 0.5; // Reduce shadow intensity on surfaces facing away
    }

    return shadow;
}

// Function to calculate directional light contribution
vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 MaterialDiffuseColor, float shadow) {
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

    // Apply shadow to diffuse and specular
    diffuse *= (1.0 - shadow);
    specular *= (1.0 - shadow);

    return ambient + diffuse + specular;
}

// Function to calculate spotlight contribution
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 MaterialDiffuseColor, float shadow) {
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

    // Apply shadow to diffuse and specular
    diffuse *= (1.0 - shadow);
    specular *= (1.0 - shadow);

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

    // Calculate shadow factor using the light space position
    float shadow = calculateShadow(FragPosLightSpace);

    // Apply directional lights if enabled
    for (int i = 0; i < numDirLights; ++i) {
        if (useDirectionalLight[i]) {
            finalColor += calcDirLight(dirLights[i], normal, viewDir, MaterialDiffuseColor, shadow);
        }
    }

    // Apply spotlights if enabled
    for (int i = 0; i < numSpotLights; ++i) {
        if (useSpotLight[i]) {
            finalColor += calcSpotLight(spotLights[i], normal, viewDir, Position_worldspace, MaterialDiffuseColor, shadow);
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
