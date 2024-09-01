#include "headers/Model.h"

// Constructor
Model::Model(const std::string& filepath) {
    loadModel(filepath);
    setupBuffers();
    loadTextures();

    // Calculate the bounding box
    glm::vec3 min, max;
    calculateBoundingBox(min, max);

    // Calculate the scale factor to fit the model within a 1.0 unit box
    glm::vec3 size = max - min;
    float maxDimension = glm::max(glm::max(size.x, size.y), size.z);
    float scaleFactor = 1.0f / maxDimension;

    // Initialize transformation attributes
    position = glm::vec3(0.0f);
    rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    rotationAngle = 0.0f;
    scale = glm::vec3(scaleFactor);
}

// Destructor to clean up OpenGL buffers
Model::~Model() {
    glDeleteBuffers(1, &vbo);
    if (!normals.empty()) {
        glDeleteBuffers(1, &nbo);
    }
    if (!texcoords.empty()) {
        glDeleteBuffers(1, &tbo);
    }
    if (!indices.empty()) {
        glDeleteBuffers(1, &ebo);
    }
    glDeleteVertexArrays(1, &vao);

    for (GLuint texture : textures) {
        glDeleteTextures(1, &texture);
    }
}

// Method to get the diffuse texture ID
GLuint Model::getTextureID(size_t materialIndex) const {
    if (materialIndex < textures.size()) {
        return textures[materialIndex];
    }
    return 0; // Return 0 if the index is out of bounds or no texture is assigned
}

// Method to get the specular texture ID
GLuint Model::getSpecularTextureID(size_t materialIndex) const {
    if (materialIndex < specularTextures.size()) {
        return specularTextures[materialIndex];
    }
    return 0; // Return 0 if the index is out of bounds or no texture is assigned
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
    std::string warn, err;

    std::string base_dir = filepath.substr(0, filepath.find_last_of('/'));
    if (!base_dir.empty()) {
        base_dir += "/";
    }

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filepath.c_str(), base_dir.c_str());

    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "ERR: " << err << std::endl;
    }

    if (!ret) {
        std::cerr << "Failed to load OBJ file: " << filepath << std::endl;
        return;
    }

    for (const auto& material : materials) {
        std::cout << "\nMaterial name: " << material.name << std::endl;
        std::cout << "Diffuse texture: " << material.diffuse_texname << std::endl;
    }

    // Process loaded data and group by materials
    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int fv = shape.mesh.num_face_vertices[f];
            int material_id = shape.mesh.material_ids[f];

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

    std::cout << "\nModel loaded successfully." << std::endl;
    std::cout << "Number of vertices: " << vertices.size() / 3 << std::endl;
    std::cout << "Number of normals: " << normals.size() / 3 << std::endl;
    std::cout << "Number of texture coordinates: " << texcoords.size() / 2 << std::endl;
    std::cout << "Number of triangles: " << indices.size() / 3<<"\n" << std::endl;
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

glm::mat4 Model::calculateModelMatrix() const {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), rotationAxis);
    glm::mat4 scaling = glm::scale(glm::mat4(1.0f), scale);
    return translation * rotation * scaling;
}

glm::mat4 Model::getModelMatrix() const {
    return calculateModelMatrix();
}

void Model::calculateBoundingBox(glm::vec3& min, glm::vec3& max) const {
    if (vertices.empty()) return;

    min = glm::vec3(vertices[0], vertices[1], vertices[2]);
    max = glm::vec3(vertices[0], vertices[1], vertices[2]);

    for (size_t i = 1; i < vertices.size() / 3; i++) {
        glm::vec3 vertex(vertices[3 * i], vertices[3 * i + 1], vertices[3 * i + 2]);
        min = glm::min(min, vertex);
        max = glm::max(max, vertex);
    }
}

void Model::draw() const {
    glBindVertexArray(vao);

    size_t indexOffset = 0;
    for (size_t i = 0; i < face_material_ids.size(); i++) {
        int material_id = face_material_ids[i];

        if (material_id >= 0 && material_id < textures.size() && textures[material_id] != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[material_id]);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture if there is none
        }

        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(indexOffset * sizeof(unsigned int)));
        indexOffset += 3;
    }

    glBindVertexArray(0);
}

void Model::setPosition(const glm::vec3& pos) {
    position = pos;
}

void Model::setRotation(float angle, const glm::vec3& axis) {
    rotationAngle = angle;
    rotationAxis = axis;
}

void Model::setScale(const glm::vec3& scl) {
    scale = scl;
}
