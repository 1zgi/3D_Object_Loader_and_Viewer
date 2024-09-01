#version 330 core

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
uniform vec3 LightPosition_worldspace; // Uniform for light position in world space

out vec4 color;

void main() {
    // Sample the diffuse texture
    vec3 MaterialDiffuseColor = texture(material.diffuseTexture, UV).rgb;

    // Apply gamma correction to the texture to ensure it works well with lighting
    MaterialDiffuseColor = pow(MaterialDiffuseColor, vec3(2.2));

    // Calculate ambient color
    vec3 MaterialAmbientColor = AmbientLightIntensity * MaterialDiffuseColor;

    // Normalize the normal vector
    vec3 n = normalize(Normal_cameraspace);

    // Calculate the direction to the light in world space
    vec3 toLight = normalize(LightPosition_worldspace - Position_worldspace);

    // Calculate the diffuse component using Lambert's cosine law
    float cosTheta = max(dot(n, toLight), 0.0);
    vec3 diffuse = dirLight.Intensity * MaterialDiffuseColor * cosTheta;

    // Calculate the specular component using the Phong reflection model
    vec3 E = normalize(EyeDirection_cameraspace);
    vec3 R = reflect(-toLight, n);
    float cosAlpha = max(dot(E, R), 0.0);
    float SpecularFactor = texture(material.specularTexture, UV).r;
    vec3 SpecularColor = material.SpecularColor * SpecularFactor * pow(cosAlpha, 32.0);

    // Calculate the point light contribution with attenuation
    vec3 toPointLight = normalize(pointLight.Position - Position_worldspace);
    float distanceToPointLight = length(pointLight.Position - Position_worldspace);
    float attenuation = 1.0 / (pointLight.Constant + pointLight.Linear * distanceToPointLight + pointLight.Quadratic * (distanceToPointLight * distanceToPointLight));
    float cosThetaPoint = max(dot(n, toPointLight), 0.0);
    vec3 pointLightColor = pointLight.Intensity * MaterialDiffuseColor * cosThetaPoint * attenuation;

    // Combine ambient, diffuse, and specular components, including point light contribution
    vec3 finalColor = MaterialAmbientColor + diffuse + pointLightColor + SpecularColor;

    // Apply gamma correction to the final color for accurate lighting
    finalColor = pow(finalColor, vec3(1.0 / 2.2));

    // Output the final color to the fragment
    color = vec4(finalColor, 1.0);
}
