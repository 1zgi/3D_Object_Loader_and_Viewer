#include "headers/Model.h"
#include <limits>  // For std::numeric_limits

// Constructor
Model::Model(const std::string& filepath) : currentFilePath(filepath) {
    // Initialize transformation attributes first
    position = glm::vec3(0.0f);
    rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    rotationAngle = 0.0f;
    scale = glm::vec3(1.0f);
    needsLowestPointUpdate = true;

    // Initialize OpenGL IDs to 0
    vao = 0;
    vbo = 0;
    nbo = 0;
    tbo = 0;
    ebo = 0;

    // Only load model if filepath is provided and not empty
    if (!filepath.empty()) {
        loadModel(filepath);
        setupBuffers();
        loadTextures();

        // Calculate the bounding box
        glm::vec3 min, max;
        calculateBoundingBox(min, max);

        // Calculate the scale factor to fit the model within a 2.0 unit box (bigger and more visible)
        glm::vec3 size = max - min;
        float maxDimension = glm::max(glm::max(size.x, size.y), size.z);
        float scaleFactor = 2.0f / maxDimension;  // Make it 2x bigger
        scale = glm::vec3(scaleFactor);
    } else {
        std::cout << "Model initialized without file. Use 'Browse Models...' to load an OBJ file." << std::endl;
    }
}

// Destructor to clean up OpenGL buffers
Model::~Model() {
    cleanup();
}

// Clean up all OpenGL resources
void Model::cleanup() {
    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (nbo && !normals.empty()) {
        glDeleteBuffers(1, &nbo);
        nbo = 0;
    }
    if (tbo && !texcoords.empty()) {
        glDeleteBuffers(1, &tbo);
        tbo = 0;
    }
    if (ebo && !indices.empty()) {
        glDeleteBuffers(1, &ebo);
        ebo = 0;
    }
    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    for (GLuint texture : textures) {
        if (texture) {
            glDeleteTextures(1, &texture);
        }
    }
    textures.clear();
    specularTextures.clear();
    
    // Clear all data vectors
    vertices.clear();
    normals.clear();
    texcoords.clear();
    indices.clear();
    materials.clear();
    face_material_ids.clear();
    diffuseColors.clear();
}

// Reload a new model at runtime
bool Model::reloadModel(const std::string& filepath) {
    try {
        std::cout << "Reloading model: " << filepath << std::endl;
        
        // Clean up existing resources
        cleanup();
        
        // Update the current file path
        currentFilePath = filepath;
        
        // Load the new model
        loadModel(filepath);
        setupBuffers();
        loadTextures();
        
        // Recalculate bounding box and scale
        glm::vec3 min, max;
        calculateBoundingBox(min, max);
        
        glm::vec3 size = max - min;
        float maxDimension = glm::max(glm::max(size.x, size.y), size.z);
        float scaleFactor = 2.0f / maxDimension;  // Make it 2x bigger
        
        // Reset transformations but keep current position and rotation
        scale = glm::vec3(scaleFactor);
        needsLowestPointUpdate = true;
        
        std::cout << "Model reloaded successfully!" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to reload model: " << e.what() << std::endl;
        return false;
    }
}

// Get current model file path
const std::string& Model::getCurrentFilePath() const {
    return currentFilePath;
}

// Get vertex count
size_t Model::getVertexCount() const {
    return vertices.size() / 3; // 3 components per vertex
}

// Get face count
size_t Model::getFaceCount() const {
    return indices.size() / 3; // 3 indices per triangle
}

// Load textures using stb_image
void Model::loadTextures() {
    std::cout << "\nStarting to load textures..." << std::endl;

    // Flip the image vertically on load
    stbi_set_flip_vertically_on_load(true);

    for (size_t i = 0; i < materials.size(); i++) {
        const auto& material = materials[i];
        if (!material.diffuse_texname.empty()) {
            std::string texPath = material.diffuse_texname;
            std::cout << "Loading texture from path: " << texPath << std::endl;

            GLuint textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            int width, height, nrComponents;
            unsigned char* data = stbi_load(texPath.c_str(), &width, &height, &nrComponents, 0);

            if (data) {
                GLenum format;
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;
                else {
                    std::cerr << "Unknown number of components in texture: " << nrComponents << std::endl;
                    stbi_image_free(data);
                    continue;
                }

                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                textures.push_back(textureID);
                std::cout << "Texture loaded and assigned ID: " << textureID << std::endl;
            }
            else {
                std::cerr << "Failed to load texture at path: " << texPath << std::endl;
                glDeleteTextures(1, &textureID);
                textures.push_back(0); // Add a placeholder texture ID
            }
            stbi_image_free(data);
        }
        else {
            std::cout << "No diffuse texture for material: " << material.name << std::endl;
            textures.push_back(0); // Add a placeholder texture ID
        }
    }
    std::cout << "Finished loading textures.\n" << std::endl;
}

// Load the model from the OBJ file using tinyobjloader
void Model::loadModel(const std::string& filepath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::string err;

    // Get base directory for material files (handle both / and \ for Windows)
    std::string base_dir = filepath.substr(0, filepath.find_last_of("/\\"));
    if (!base_dir.empty()) {
        base_dir += "/";
    }

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filepath.c_str(), base_dir.c_str());

    if (!err.empty()) {
        std::cerr << "ERR: " << err << std::endl;
    }

    if (!ret) {
        std::cerr << "Failed to load OBJ file: " << filepath << std::endl;
        return;
    }

    // If no materials loaded, create a default material
    if (materials.empty()) {
        std::cout << "No materials found. Creating default material." << std::endl;
        tinyobj::material_t defaultMat;
        defaultMat.name = "default";
        defaultMat.diffuse[0] = 0.8f; // Light gray
        defaultMat.diffuse[1] = 0.8f;
        defaultMat.diffuse[2] = 0.8f;
        defaultMat.ambient[0] = 0.2f;
        defaultMat.ambient[1] = 0.2f;
        defaultMat.ambient[2] = 0.2f;
        materials.push_back(defaultMat);
    }

    // Store diffuse colors for materials
    for (const auto& material : materials) {
        glm::vec3 diffuseColor(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        diffuseColors.push_back(diffuseColor);
    }

    // Process loaded data and group by materials
    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int fv = shape.mesh.num_face_vertices[f];
            int material_id = shape.mesh.material_ids[f];
            
            // If no material assigned (-1), use first material (default)
            if (material_id < 0) {
                material_id = 0;
            }

            face_material_ids.push_back(material_id);

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                if (idx.vertex_index >= 0 && idx.vertex_index < attrib.vertices.size() / 3) {
                    vertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
                    vertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
                    vertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
                }

                if (idx.normal_index >= 0 && idx.normal_index < attrib.normals.size() / 3) {
                    normals.push_back(attrib.normals[3 * idx.normal_index + 0]);
                    normals.push_back(attrib.normals[3 * idx.normal_index + 1]);
                    normals.push_back(attrib.normals[3 * idx.normal_index + 2]);
                }

                if (idx.texcoord_index >= 0 && idx.texcoord_index < attrib.texcoords.size() / 2) {
                    texcoords.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
                    texcoords.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
                }

                indices.push_back(static_cast<unsigned int>(indices.size()));
            }
            index_offset += fv;
        }
    }

    std::cout << "Model loaded successfully.\n" << std::endl;
}

// Setup OpenGL buffers
void Model::setupBuffers() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generate and bind VBO for vertex positions
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); // Bind to location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Generate and bind NBO for normals, if they exist
    if (!normals.empty()) {
        glGenBuffers(1, &nbo);
        glBindBuffer(GL_ARRAY_BUFFER, nbo);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1); // Bind to location 1
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    // Generate and bind TBO for texture coordinates, if they exist
    if (!texcoords.empty()) {
        glGenBuffers(1, &tbo);
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(float), &texcoords[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2); // Bind to location 2
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);  // Ensure 2 components (u, v)
    }

    // Generate and bind EBO for indices
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0); // Unbind the VAO
}

// Transformation matrix calculation
glm::mat4 Model::calculateModelMatrix() const {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), rotationAxis);
    glm::mat4 scaling = glm::scale(glm::mat4(1.0f), scale);
    return translation * rotation * scaling;
}

void Model::calculateBoundingBox(glm::vec3& min, glm::vec3& max) const { //Fits the model within a 1.0 unit box
    if (vertices.empty()) return;

    min = glm::vec3(vertices[0], vertices[1], vertices[2]);
    max = glm::vec3(vertices[0], vertices[1], vertices[2]);

    for (size_t i = 1; i < vertices.size() / 3; i++) {
        glm::vec3 vertex(vertices[3 * i], vertices[3 * i + 1], vertices[3 * i + 2]);
        min = glm::min(min, vertex);
        max = glm::max(max, vertex);
    }
}

// Method to render the model
void Model::draw(GLuint programID) const {
    // Don't draw if no model is loaded
    if (vertices.empty() || indices.empty() || vao == 0) {
        return;
    }



    // Bind the VAO for the model
    glBindVertexArray(vao);

    // Retrieve the indices and face material IDs
    const std::vector<unsigned int>& modelIndices = getIndices();
    const std::vector<int>& materialIDs = getFaceMaterialIDs();

    // Retrieve the model materials
    const std::vector<MaterialData>& materialsData = getModelMaterials();

    GLuint currentTextureID = 0;  // Track the current bound texture to avoid redundant binding
    glm::vec3 currentDiffuseColor = glm::vec3(-1.0f);  // Start with invalid color to force first update
    glm::vec3 currentSpecularColor = glm::vec3(-1.0f); // Start with invalid specular color
    float currentShininess = -1.0f;  // Invalid shininess to force first update

    size_t indexOffset = 0;  // Offset for the indices
    size_t faceStart = 0;    // To batch faces that share the same material

    // Iterate over each material group
    for (size_t i = 0; i < materialIDs.size(); ++i) {
        int materialID = materialIDs[i];

        // Ensure the material ID is valid
        if (materialID >= 0 && materialID < materialsData.size()) {
            const MaterialData& mat = materialsData[materialID];

            // If the material has a texture and it's different from the current texture, bind it
            if (mat.diffuseTextureID != 0 && mat.diffuseTextureID != currentTextureID) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mat.diffuseTextureID);
                glUniform1i(glGetUniformLocation(programID, "useTexture"), 1);  // Use the texture
                currentTextureID = mat.diffuseTextureID;
            }
            // If there's no texture, use the diffuse color
            else if (mat.diffuseTextureID == 0) {
                if (currentTextureID != 0) {
                    glBindTexture(GL_TEXTURE_2D, 0);  // Unbind texture if previously bound
                    glUniform1i(glGetUniformLocation(programID, "useTexture"), 0);  // Use diffuse color instead of texture
                    currentTextureID = 0;
                }

                // Set the diffuse color in the shader if it has changed
                if (mat.diffuseColor != currentDiffuseColor) {
                    glUniform3fv(glGetUniformLocation(programID, "material.DiffuseColor"), 1, &mat.diffuseColor[0]);
                    currentDiffuseColor = mat.diffuseColor;
                }
            }

            // Set specular color and shininess only if they have changed
            if (mat.specularColor != currentSpecularColor) {
                glUniform3fv(glGetUniformLocation(programID, "material.SpecularColor"), 1, &mat.specularColor[0]);
                currentSpecularColor = mat.specularColor;
            }

            if (mat.shininess != currentShininess) {
                glUniform1f(glGetUniformLocation(programID, "material.Shininess"), mat.shininess);
                currentShininess = mat.shininess;
            }

            // Check if the next face has a different material ID to batch the draw calls
            if (i + 1 == materialIDs.size() || materialIDs[i + 1] != materialID) {
                // Draw all faces that use the same material in a single draw call
                size_t numFaces = (i + 1 - faceStart) * 3;  // Number of vertices for this material group

                // Instead of passing size_t directly, cast it to GLsizei
                glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(numFaces), GL_UNSIGNED_INT, (void*)(indexOffset * sizeof(unsigned int)));

                // Update the index offset and face start position
                indexOffset += numFaces;
                faceStart = i + 1;
            }
        }
    }

    // Unbind the VAO
    glBindVertexArray(0);
}

bool Model::isLowestPointUpdateNeeded() const {
    return needsLowestPointUpdate;
}

GLuint Model::getVAO() const {
    return vao;
}

// Get the indices of the model
const std::vector<unsigned int>& Model::getIndices() const {
    return indices;
}

// Get the material IDs for each face
const std::vector<int>& Model::getFaceMaterialIDs() const {
    return face_material_ids;
}

glm::vec3 Model::getMaterialDiffuseColor(size_t materialIndex) const {
    if (materialIndex < diffuseColors.size()) {
        return diffuseColors[materialIndex];
    }
    return glm::vec3(1.0f, 1.0f, 1.0f);  // Return white if index is out of bounds
}

std::vector<MaterialData> Model::getModelMaterials() const {
    std::vector<MaterialData> materialsData;

    for (size_t i = 0; i < materials.size(); i++) {
        MaterialData data;

        // Set diffuse texture ID if available
        if (i < textures.size() && textures[i] != 0) {
            data.diffuseTextureID = textures[i];

            // Use the material's diffuse color, but fallback to white if not properly defined
            data.diffuseColor = glm::vec3(materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
        }
        else {
            data.diffuseTextureID = 0;  // No texture assigned, use diffuse color instead
            data.diffuseColor = glm::vec3(materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);

            // If the diffuse color in material is zero (default case), fallback to white
            if (data.diffuseColor == glm::vec3(0.0f, 0.0f, 0.0f)) {
                data.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);  // Default to white color
            }
        }

        // Set specular texture ID if available
        if (i < specularTextures.size() && specularTextures[i] != 0) {
            data.specularTextureID = specularTextures[i];

            // Use the material's specular color, but fallback to white if not properly defined
            data.specularColor = glm::vec3(materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
            data.shininess = (materials[i].shininess > 0.0f) ? materials[i].shininess : 64.0f;
        }
        else {
            data.specularTextureID = 0;  // No specular texture assigned
            data.specularColor = glm::vec3(0.5f, 0.5f, 0.5f);  // Default to white specular color
            data.shininess = 64.0f;  // Default shininess if none is set
        }

        materialsData.push_back(data);
    }

    return materialsData;
}

glm::mat4 Model::getModelMatrix() const {
    return calculateModelMatrix();
}

// Efficiently calculates the lowest point (y-coordinate) of the model
float Model::getLowestPoint() const {
    if (needsLowestPointUpdate) {
        updateLowestPoint();
    }
    return lowestPoint;
}

void Model::updateLowestPoint() const {
    // Only recalculate if transformations have changed
    if (!needsLowestPointUpdate) {
        return;
    }

    float minY = std::numeric_limits<float>::max();

    for (size_t i = 0; i < vertices.size() / 3; i++) {
        glm::vec3 vertex(vertices[3 * i], vertices[3 * i + 1], vertices[3 * i + 2]);

        // Apply model transformations to the vertex
        glm::vec4 transformedVertex = getModelMatrix() * glm::vec4(vertex, 1.0f);

        // Check for the minimum Y value after transformation
        if (transformedVertex.y < minY) {
            minY = transformedVertex.y;
        }
    }

    lowestPoint = minY;
    needsLowestPointUpdate = false;  // Reset the flag after updating
}

GLuint Model::getTextureID(size_t materialIndex) const {
    if (materialIndex < textures.size()) {
        return textures[materialIndex];
    }
    return 0; // Return 0 if the index is out of bounds or no texture is assigned
}

GLuint Model::getSpecularTextureID(size_t materialIndex) const {
    if (materialIndex < specularTextures.size()) {
        return specularTextures[materialIndex];
    }
    return 0; // Return 0 if the index is out of bounds or no texture is assigned
}

glm::vec3 Model::getPosition() const {
    return position;
}

void Model::setPosition(const glm::vec3& pos) {
    position = pos;
    needsLowestPointUpdate = true;
}

void Model::setRotation(float angle, const glm::vec3& axis) {
    rotationAngle = angle;
    rotationAxis = axis;
}

void Model::setScale(const glm::vec3& scl) {
    scale = scl;
}