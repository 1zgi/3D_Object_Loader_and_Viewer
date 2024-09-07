#version 410 core

in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
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
    vec3 Intensity;
};

struct PointLight {
    vec3 Position;
    vec3 Intensity;
    float Constant;
    float Linear;
    float Quadratic;
};

uniform Material material;
uniform DirectionalLight dirLight;
uniform PointLight pointLight;
uniform vec3 AmbientLightIntensity;
uniform vec3 LightPosition_worldspace;

uniform bool useTexture;  // Correctly check if we should use the texture

out vec4 color;

void main() {
    vec3 MaterialDiffuseColor;

    // Use texture if available, otherwise fallback to default white color
    if (useTexture) {  // Use texture when useTexture is true
        MaterialDiffuseColor = texture(material.diffuseTexture, UV).rgb;
        MaterialDiffuseColor = pow(MaterialDiffuseColor, vec3(2.2));  // Apply gamma correction
    } else {
        MaterialDiffuseColor = material.DiffuseColor;  // Default white color
    }

    // Lighting calculations...
    vec3 MaterialAmbientColor = AmbientLightIntensity * MaterialDiffuseColor;

    // Normalize the normal vector
    vec3 n = normalize(Normal_cameraspace);
    vec3 toLight = normalize(LightPosition_worldspace - Position_worldspace);

    float cosTheta = max(dot(n, toLight), 0.0);
    vec3 diffuse = dirLight.Intensity * MaterialDiffuseColor * cosTheta;

    vec3 E = normalize(EyeDirection_cameraspace);
    vec3 R = reflect(-toLight, n);
    float cosAlpha = max(dot(E, R), 0.0);

    float SpecularFactor;
    if (useTexture) {  // Check for specular texture as well
        SpecularFactor = texture(material.specularTexture, UV).r;
    } else {
        SpecularFactor = 0.5;  // Use default specular factor when no texture
    }

    vec3 SpecularColor = material.SpecularColor * SpecularFactor * pow(cosAlpha, 32.0);

    vec3 toPointLight = normalize(pointLight.Position - Position_worldspace);
    float distanceToPointLight = length(pointLight.Position - Position_worldspace);
    float attenuation = 1.0 / (pointLight.Constant + pointLight.Linear * distanceToPointLight + pointLight.Quadratic * (distanceToPointLight * distanceToPointLight));
    float cosThetaPoint = max(dot(n, toPointLight), 0.0);
    vec3 pointLightColor = pointLight.Intensity * MaterialDiffuseColor * cosThetaPoint * attenuation;

    vec3 finalColor = MaterialAmbientColor + diffuse + pointLightColor + SpecularColor;

    finalColor = pow(finalColor, vec3(1.0 / 2.2));  // Apply gamma correction

    color = vec4(finalColor, 1.0);
}
