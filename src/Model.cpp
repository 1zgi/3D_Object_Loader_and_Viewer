#include "headers/Model.h"

// Constructor
Model::Model(const std::string& filepath) {
    loadModel(filepath);
    setupBuffers();
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
    glDeleteVertexArrays(1, &vao);
}

// Load the model from the OBJ file using tinyobjloader
void Model::loadModel(const std::string& filepath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filepath.c_str());

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
    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                // Debugging vertex indices
                if (idx.vertex_index >= 0 && idx.vertex_index < attrib.vertices.size() / 3) {
                    vertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
                    vertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
                    vertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
                }
                else {
                    std::cerr << "Vertex index out of range: " << idx.vertex_index << std::endl;
                }

                // Debugging normal indices
                if (idx.normal_index >= 0 && idx.normal_index < attrib.normals.size() / 3) {
                    normals.push_back(attrib.normals[3 * idx.normal_index + 0]);
                    normals.push_back(attrib.normals[3 * idx.normal_index + 1]);
                    normals.push_back(attrib.normals[3 * idx.normal_index + 2]);
                }
                else {
                    std::cerr << "Normal index out of range: " << idx.normal_index << std::endl;
                }

                // Debugging texture coordinate indices
                if (idx.texcoord_index >= 0 && idx.texcoord_index < attrib.texcoords.size() / 2) {
                    texcoords.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
                    texcoords.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
                }
                else {
                    std::cerr << "Texture coordinate index out of range: " << idx.texcoord_index << std::endl;
                }
            }
            index_offset += fv;
        }
    }

    std::cout << "Model loaded successfully." << std::endl;
    std::cout << "Number of vertices: " << vertices.size() / 3 << std::endl;
    std::cout << "Number of normals: " << normals.size() / 3 << std::endl;
    std::cout << "Number of texture coordinates: " << texcoords.size() / 2 << std::endl;
}

// Setup OpenGL buffers
void Model::setupBuffers() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    std::cout << "VBO size: " << vertices.size() << std::endl;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    if (!normals.empty()) {
        glGenBuffers(1, &nbo);
        glBindBuffer(GL_ARRAY_BUFFER, nbo);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
        std::cout << "NBO size: " << normals.size() << std::endl;
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
    }

    if (!texcoords.empty()) {
        glGenBuffers(1, &tbo);
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(float), &texcoords[0], GL_STATIC_DRAW);
        std::cout << "TBO size: " << texcoords.size() << std::endl;
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


// Draw the model
void Model::draw() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
    glBindVertexArray(0);
}
