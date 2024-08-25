#include "headers/Model.h"

// Constructor
Model::Model(const std::string& filepath) {
    loadModel(filepath);
    setupBuffers();

    // Initialize transformation attributes
    position = glm::vec3(0.0f);
    rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    rotationAngle = 0.0f;
    scale = glm::vec3(1.0f);
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
}

// Load the model from the OBJ file using tinyobjloader
void Model::loadModel(const std::string& filepath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
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

    // Process loaded data
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            if (index.vertex_index >= 0 && index.vertex_index < attrib.vertices.size() / 3) {
                vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
                vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
                vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);
            }

            if (index.normal_index >= 0 && index.normal_index < attrib.normals.size() / 3) {
                normals.push_back(attrib.normals[3 * index.normal_index + 0]);
                normals.push_back(attrib.normals[3 * index.normal_index + 1]);
                normals.push_back(attrib.normals[3 * index.normal_index + 2]);
            }

            if (index.texcoord_index >= 0 && index.texcoord_index < attrib.texcoords.size() / 2) {
                texcoords.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                texcoords.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
            }

            indices.push_back(static_cast<unsigned int>(indices.size()));
        }
    }

    this->materials = materials; // Store the materials for later use

    std::cout << "Model loaded successfully." << std::endl;
    std::cout << "Number of vertices: " << vertices.size() / 3 << std::endl;
    std::cout << "Number of normals: " << normals.size() / 3 << std::endl;
    std::cout << "Number of texture coordinates: " << texcoords.size() / 2 << std::endl;
    std::cout << "Number of triangles: " << indices.size() / 3 << std::endl;
}

// Setup OpenGL buffers
void Model::setupBuffers() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    if (!normals.empty()) {
        glGenBuffers(1, &nbo);
        glBindBuffer(GL_ARRAY_BUFFER, nbo);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
    }

    if (!texcoords.empty()) {
        glGenBuffers(1, &tbo);
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(float), &texcoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);
    }

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
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

void Model::draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
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
