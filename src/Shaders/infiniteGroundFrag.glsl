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

// Uniform variables for the light and material
uniform DirectionalLight dirLight;
uniform SpotLight spotLight;

uniform bool useDirectionalLight;  // Flag to enable/disable directional light
uniform bool useSpotLight;         // Flag to enable/disable spotlight

uniform vec3 materialDiffuseColor;  // Material diffuse color
uniform vec3 materialSpecularColor; // Material specular color
uniform float materialShininess;

uniform vec3 viewPos;               // Camera position

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
    // Normalize the normal vector
    vec3 normal = normalize(Normal_cameraspace);

    // View direction (from fragment to camera)
    vec3 viewDir = normalize(viewPos - FragPos_worldspace);

    // Initialize the final color
    vec3 result = vec3(0.0);

    // Apply directional light if enabled
    if (useDirectionalLight) {
        result += calcDirLight(dirLight, normal, viewDir);
    }

    // Apply spotlight if enabled
    if (useSpotLight) {
        result += calcSpotLight(spotLight, normal, FragPos_worldspace, viewDir);
    }

    // Set final fragment color
    FragColor = vec4(result, 1.0);
}
