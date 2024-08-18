#include "headers/shader.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {
    // Create shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read Vertex Shader code from file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }
    else {
        std::cerr << "Impossible to open " << vertex_file_path << ". Are you in the right directory?" << std::endl;
        return 0;
    }

    // Read Fragment Shader code from file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }
    else {
        std::cerr << "Impossible to open " << fragment_file_path << ". Are you in the right directory?" << std::endl;
        return 0;
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    std::cout << "Compiling vertex shader: " << vertex_file_path << std::endl;
    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        std::cerr << "Vertex Shader Error: " << &VertexShaderErrorMessage[0] << std::endl;
    }
    if (Result == GL_FALSE) {
        std::cerr << "Failed to compile vertex shader!" << std::endl;
        return 0;
    }

    // Compile Fragment Shader
    std::cout << "Compiling fragment shader: " << fragment_file_path << std::endl;
    char const* FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        std::cerr << "Fragment Shader Error: " << &FragmentShaderErrorMessage[0] << std::endl;
    }
    if (Result == GL_FALSE) {
        std::cerr << "Failed to compile fragment shader!" << std::endl;
        return 0;
    }

    // Link the program
    std::cout << "Linking program" << std::endl;
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        std::cerr << "Shader Program Linking Error: " << &ProgramErrorMessage[0] << std::endl;
    }
    if (Result == GL_FALSE) {
        std::cerr << "Failed to link shader program!" << std::endl;
        return 0;
    }

    // Cleanup shaders
    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}
