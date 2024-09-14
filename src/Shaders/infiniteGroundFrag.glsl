#version 410 core

in vec3 Normal_cameraspace;
in vec3 FragPos_worldspace;

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

uniform vec3 materialDiffuseColor;
uniform vec3 materialSpecularColor;
uniform float materialShininess;
uniform vec3 viewPos;

uniform bool useSpotLight[MAX_LIGHTS];           // Toggle for each spotlight
uniform bool useDirectionalLight[MAX_LIGHTS];    // Toggle for each directional light

// Function to calculate directional light contribution
vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.Direction);
    
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.Intensity * diff * materialDiffuseColor;

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    vec3 specular = light.Specular * spec * materialSpecularColor;

    // Ambient shading
    vec3 ambient = light.Ambient * materialDiffuseColor;

    return ambient + diffuse + specular;
}

// Function to calculate spotlight contribution
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
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

    return ambient + diffuse + specular;
}

void main() {
    vec3 normal = normalize(Normal_cameraspace);
    vec3 viewDir = normalize(viewPos - FragPos_worldspace);
    vec3 result = vec3(0.0);

    // Apply all directional lights
    for (int i = 0; i < numDirLights; i++) {
        if (useDirectionalLight[i]) {
            result += calcDirLight(dirLights[i], normal, viewDir);
        }
    }

    // Apply all spotlights
    for (int i = 0; i < numSpotLights; i++) {
        if (useSpotLight[i]) {
            result += calcSpotLight(spotLights[i], normal, FragPos_worldspace, viewDir);
        }
    }

    // Set the final fragment color
    FragColor = vec4(result, 1.0);
}

