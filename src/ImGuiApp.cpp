#include "headers/ImGuiApp.h"
#include <algorithm>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#undef byte  // Prevent conflicts with std::byte
#endif

ImGuiApp::ImGuiApp() : window(nullptr), done(false) {
    // Initialize current directory to current working directory
    currentDirectory = ".";
    refreshDirectoryContents();
}

ImGuiApp::~ImGuiApp() {
    Cleanup();
}

bool ImGuiApp::Init(Window* window) {
    this->window = window;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    if (!ImGui_ImplSDL2_InitForOpenGL(window->getWindow(), window->getContext())) {
        std::cerr << "Failed to initialize ImGui SDL2 backend!" << std::endl;
        return false;
    }
    if (!ImGui_ImplOpenGL3_Init("#version 410")) {
        std::cerr << "Failed to initialize ImGui OpenGL3 backend!" << std::endl;
        return false;
    }

    return true;
}

void ImGuiApp::Run(Renderer* renderer, Model* model) {
   
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    glm::vec3 currentAmbientIntensity = renderer->getAmbientLightIntensity();

    // Calculate the scalar intensity as the average of the components(or use length for different logic)
    float ambientIntensity = (currentAmbientIntensity.x + currentAmbientIntensity.y + currentAmbientIntensity.z) / 3.0f;

    // Add controls for ambient light intensity and model rotation
    ImGui::Begin("Ambient Light Control");
    if (ImGui::SliderFloat("Ambient Intensity", &ambientIntensity, 0.0f, 1.0f)) {
      // Update ambientLightIntensity uniformly with the new value
      renderer->setAmbientLightIntensity(glm::vec3(ambientIntensity));
    }
    
    ImGui::Separator();
    
    // Auto rotation controls
    bool autoRotate = renderer->getAutoRotation();
    if (ImGui::Checkbox("Auto Rotate Model", &autoRotate)) {
        renderer->setAutoRotation(autoRotate);
    }
    
    if (autoRotate) {
        static float rotSpeed = 30.0f;
        if (ImGui::SliderFloat("Rotation Speed (°/s)", &rotSpeed, 5.0f, 120.0f)) {
            renderer->setRotationSpeed(rotSpeed);
        }
    }
    
    ImGui::Separator();
    
    // Shadow controls
    bool shadowsEnabled = renderer->getShadowsEnabled();
    if (ImGui::Checkbox("Enable Shadows", &shadowsEnabled)) {
        renderer->setShadowsEnabled(shadowsEnabled);
    }
    
    ImGui::Separator();
    
    // Model loading controls
    ImGui::Text("Model Loading:");
    
    if (ImGui::Button("Browse Models...")) {
        std::string selectedFile = showFileDialog();
        if (!selectedFile.empty()) {
            loadingStatus.clear();
            isLoading = true;
            
            if (model->reloadModel(selectedFile)) {
                loadingStatus = "Model loaded successfully!";
            } else {
                loadingStatus = "Error: Failed to load model!";
            }
            
            isLoading = false;
        }
    }
    
    // Display current model info
    std::string currentPath = model->getCurrentFilePath();
    if (currentPath.empty()) {
        ImGui::Text("Current: No model loaded");
        ImGui::Text("Use 'Browse Models...' or drag & drop an OBJ file");
    } else {
        ImGui::Text("Current: %s", currentPath.c_str());
        ImGui::Text("Vertices: %zu, Faces: %zu", model->getVertexCount(), model->getFaceCount());
    }
    
    // Show loading status
    if (isLoading) {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Loading...");
    } else if (!loadingStatus.empty()) {
        ImVec4 color = loadingStatus.find("Error") != std::string::npos ? 
                      ImVec4(1, 0, 0, 1) : ImVec4(0, 1, 0, 1);
        ImGui::TextColored(color, "%s", loadingStatus.c_str());
    }
    
    ImGui::End();

   // Render your scene
   renderer->renderScene();

   // Rendering ImGui
   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

   // Swap buffers
   SDL_GL_SwapWindow(renderer->getWindow().getWindow());

   // Check for OpenGL errors
   GLenum err;
   while ((err = glGetError()) != GL_NO_ERROR) {
      std::cerr << "OpenGL error: " << err << std::endl;
   } 
}

void ImGuiApp::Cleanup() {
    // Cleanup ImGui
    if (ImGui::GetCurrentContext()) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    // Note: Cleanup SDL only if this class is responsible for it.
    if (window) {
        // Avoid deleting the SDL context and window if they're managed elsewhere
        window = nullptr;
    }

    // Avoid calling SDL_Quit here if it's managed elsewhere in your application.
    // SDL_Quit();
}

// Helper method to check if a file is an OBJ file
bool ImGuiApp::isObjFile(const std::string& filename) {
    std::string ext = getFileExtension(filename);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".obj";
}

// Helper method to get file extension
std::string ImGuiApp::getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        return filename.substr(dotPos);
    }
    return "";
}

// Function to show Windows file dialog and get OBJ file path
std::string ImGuiApp::showFileDialog() {
#ifdef _WIN32
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "OBJ Files\0*.obj\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    ofn.lpstrTitle = "Select OBJ Model File";
    
    if (GetOpenFileNameA(&ofn)) {
        return std::string(szFile);
    }
#endif
    return "";
}

// Refresh directory contents using Windows API (avoiding std::filesystem conflicts)
void ImGuiApp::refreshDirectoryContents() {
    directoryContents.clear();
    
    // Add parent directory option if not at root
    if (currentDirectory != "." && currentDirectory != "/" && currentDirectory != "C:\\" && !currentDirectory.empty()) {
        directoryContents.push_back("..");
    }
    
#ifdef _WIN32
    // Use Windows API to list directory contents
    std::string searchPath = currentDirectory;
    if (searchPath.back() != '\\' && searchPath.back() != '/') {
        searchPath += "\\";
    }
    searchPath += "*";
    
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            std::string fileName = findData.cFileName;
            
            // Skip current and parent directory entries
            if (fileName == "." || fileName == "..") {
                continue;
            }
            
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // It's a directory
                directoryContents.push_back("[DIR] " + fileName);
            } else if (isObjFile(fileName)) {
                // It's an OBJ file
                directoryContents.push_back(fileName);
            }
        } while (FindNextFileA(hFind, &findData));
        
        FindClose(hFind);
    }
#endif
    
    // If no files found (except parent dir), add a placeholder
    size_t expectedMinSize = (currentDirectory != "." && currentDirectory != "/" && currentDirectory != "C:\\" && !currentDirectory.empty()) ? 1 : 0;
    if (directoryContents.size() <= expectedMinSize) {
        directoryContents.push_back("(No .obj files found in this directory)");
    }
}

// Render file browser window
void ImGuiApp::renderFileBrowser(Model* model) {
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin("Model Browser", &showFileBrowser)) {
        ImGui::Text("Directory: %s", currentDirectory.c_str());
        
        if (ImGui::Button("Refresh")) {
            refreshDirectoryContents();
        }
        
        ImGui::Separator();
        
        // File list
        ImGui::BeginChild("FileList", ImVec2(0, -60), true);
        
        for (const std::string& item : directoryContents) {
            bool isSelected = (selectedFile == item);
            
            if (ImGui::Selectable(item.c_str(), isSelected)) {
                selectedFile = item;
                
                if (item == "..") {
                    // Go to parent directory
                    size_t lastSlash = currentDirectory.find_last_of("/\\");
                    if (lastSlash != std::string::npos) {
                        currentDirectory = currentDirectory.substr(0, lastSlash);
                        if (currentDirectory.empty()) currentDirectory = ".";
                    }
                    refreshDirectoryContents();
                    selectedFile.clear();
                } else if (item.find("[DIR]") == 0) {
                    // Enter directory
                    std::string dirName = item.substr(6); // Remove "[DIR] " prefix
                    if (currentDirectory.back() != '/' && currentDirectory.back() != '\\') {
                        currentDirectory += "/";
                    }
                    currentDirectory += dirName;
                    refreshDirectoryContents();
                    selectedFile.clear();
                }
            }
            
            // Double-click to load OBJ file
            if (isSelected && ImGui::IsMouseDoubleClicked(0) && isObjFile(item) && 
                item.find("(No .obj files found") == std::string::npos) {
                std::string fullPath = currentDirectory;
                if (fullPath.back() != '/' && fullPath.back() != '\\') {
                    fullPath += "/";
                }
                fullPath += item;
                loadingStatus.clear();
                isLoading = true;
                
                if (model->reloadModel(fullPath)) {
                    loadingStatus = "Model loaded successfully!";
                    showFileBrowser = false;
                } else {
                    loadingStatus = "Error: Failed to load model!";
                }
                isLoading = false;
            }
        }
        
        ImGui::EndChild();
        
        ImGui::Separator();
        
        // Bottom buttons
        if (ImGui::Button("Load Selected") && !selectedFile.empty() && isObjFile(selectedFile) && 
            selectedFile.find("(No .obj files found") == std::string::npos) {
            std::string fullPath = currentDirectory;
            if (fullPath.back() != '/' && fullPath.back() != '\\') {
                fullPath += "/";
            }
            fullPath += selectedFile;
            loadingStatus.clear();
            isLoading = true;
            
            if (model->reloadModel(fullPath)) {
                loadingStatus = "Model loaded successfully!";
                showFileBrowser = false;
            } else {
                loadingStatus = "Error: Failed to load model!";
            }
            isLoading = false;
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            showFileBrowser = false;
        }
        
        if (!selectedFile.empty()) {
            ImGui::Text("Selected: %s", selectedFile.c_str());
        }
    }
    ImGui::End();
}
