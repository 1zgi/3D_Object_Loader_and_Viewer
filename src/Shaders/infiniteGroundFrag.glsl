#version 410 core

in vec3 Normal_cameraspace;
in vec3 FragPos_worldspace;
in vec4 FragPosLightSpace;  // Position in light space for shadow mapping

out vec4 FragColor;

struct DirectionalLight {
    vec3 Direction;
    vec3 Intensity;
    vec3 Ambient;
    vec3 Specular;
};

struct SpotLight {
    vec3 Position;
    vec3 Direction;
    vec3 Intensity;
    vec3 Ambient;
    vec3 Specular;
    float CutOff;
    float OuterCutOff;
    float Constant;
    float Linear;
    float Quadratic;
};

// Maximum number of lights
const int MAX_LIGHTS = 4;

// Arrays of lights
uniform DirectionalLight dirLights[MAX_LIGHTS];
uniform SpotLight spotLights[MAX_LIGHTS];

// Number of active lights
uniform int numDirLights;
uniform int numSpotLights;

// Shadow map and bias for shadow calculation
uniform sampler2D shadowMap;  // Shadow map texture
uniform float shadowBias;     // Bias to reduce shadow acne
uniform bool shadowsEnabled;  // Flag to enable/disable shadows

// Material properties
uniform vec3 materialDiffuseColor;
uniform vec3 materialSpecularColor;
uniform float materialShininess;
uniform vec3 viewPos;

uniform bool useDirectionalLight[MAX_LIGHTS];    // Toggle for each directional light
uniform bool useSpotLight[MAX_LIGHTS];           // Toggle for each spotlight

// Function to calculate shadow factor with PCF (Percentage Closer Filtering)
float calculateShadow(vec4 fragPosLightSpace) {
    // If shadows are disabled, return no shadow
    if (!shadowsEnabled) {
        return 0.0;
    }

    // Perform perspective division (convert from clip space to NDC)
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
    
    // Calculate bias to prevent shadow acne - ground usually receives shadows well
    float bias = max(shadowBias * (1.0 - dot(normal, lightDir)), shadowBias * 0.1);
    
    // PCF (Percentage Closer Filtering) for soft shadows
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    // Sample a 3x3 grid around the current texel for smooth shadows
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }    
    }
    shadow /= 9.0; // Average the 9 samples

    return shadow;
}

// Function to calculate directional light contribution
vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, float shadow) {
    vec3 lightDir = normalize(-light.Direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.Intensity * diff * materialDiffuseColor;

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    vec3 specular = light.Specular * spec * materialSpecularColor;

    // Apply shadow factor to the specular contribution to make shadow similar to specular map
    specular *= (1.0 - shadow);

    // Ambient shading
    vec3 ambient = light.Ambient * materialDiffuseColor;

    return ambient + diffuse + specular;
}

// Function to calculate spotlight contribution
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow) {
    vec3 lightDir = normalize(light.Position - fragPos);
    float theta = dot(lightDir, normalize(-light.Direction));
    float epsilon = light.CutOff - light.OuterCutOff;
    float spotlightIntensity = clamp((theta - light.OuterCutOff) / epsilon, 0.0, 1.0);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.Intensity * diff * materialDiffuseColor;

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    vec3 specular = light.Specular * spec * materialSpecularColor;

    // Ambient shading
    vec3 ambient = light.Ambient * materialDiffuseColor;

    // Attenuation based on distance
    float distance = length(light.Position - fragPos);
    float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * (distance * distance));

    ambient *= attenuation * spotlightIntensity;
    diffuse *= attenuation * spotlightIntensity;
    specular *= attenuation * spotlightIntensity;

    // Apply shadow factor to diffuse and specular lighting
    diffuse *= (1.0 - shadow);
    specular *= (1.0 - shadow);

    return ambient + diffuse + specular;
}

void main() {
    vec3 normal = normalize(Normal_cameraspace);
    vec3 viewDir = normalize(viewPos - FragPos_worldspace);
    vec3 result = vec3(0.0);

    // Calculate shadow factor using the light space position
    float shadow = calculateShadow(FragPosLightSpace);

    // Apply all directional lights
    for (int i = 0; i < numDirLights; i++) {
        if (useDirectionalLight[i]) {
            result += calcDirLight(dirLights[i], normal, viewDir, shadow);
        }
    }

    // Apply all spotlights
    for (int i = 0; i < numSpotLights; i++) {
        if (useSpotLight[i]) {
            result += calcSpotLight(spotLights[i], normal, FragPos_worldspace, viewDir, shadow);
        }
    }

    // Set the final fragment color
    FragColor = vec4(result, 1.0);
}