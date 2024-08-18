#ifndef SHADER_HPP
#define SHADER_HPP
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <GL/glew.h>
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace std;

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

#endif