cmake_minimum_required(VERSION 3.20)
project(3D_Object_Loader_and_Viewer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

# Homebrew packages (add -DCMAKE_PREFIX_PATH=/opt/homebrew if needed)
find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(3D_Object_Loader_and_Viewer
        # Your sources
        src/glad.c
        src/Camera.cpp
        src/ImGuiApp.cpp
        src/InfiniteGround.cpp
        src/Lights.cpp
        src/main.cpp
        src/Model.cpp
        src/Renderer.cpp
        src/shader.cpp
        src/ShadowMap.cpp
        src/stb.cpp
        src/tiny_obj_loader.cc
        src/Window.cpp

        # ImGui core + SDL2 + OpenGL3 backends
        lib/imgui/imgui.cpp
        lib/imgui/imgui_draw.cpp
        lib/imgui/imgui_tables.cpp
        lib/imgui/imgui_widgets.cpp
        lib/imgui/imgui_demo.cpp
        lib/imgui/imgui_impl_sdl2.cpp
        lib/imgui/imgui_impl_opengl3.cpp
)

# ---- tinyfiledialogs (for macOS/Linux Browse dialog)
target_sources(3D_Object_Loader_and_Viewer PRIVATE external/tfd/tinyfiledialogs.c)
target_include_directories(3D_Object_Loader_and_Viewer PRIVATE ${CMAKE_SOURCE_DIR}/external/tfd)

# Your header search paths
target_include_directories(3D_Object_Loader_and_Viewer PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib/imgui
        ${CMAKE_SOURCE_DIR}/src/headers
)

# Link libs
target_link_libraries(3D_Object_Loader_and_Viewer PRIVATE
        SDL2::SDL2
        OpenGL::GL
        glm::glm
)

# ImGui OpenGL loader = GLAD (recommended)
target_compile_definitions(3D_Object_Loader_and_Viewer PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

# Warnings
if(MSVC)
    target_compile_options(3D_Object_Loader_and_Viewer PRIVATE /W4)
else()
    target_compile_options(3D_Object_Loader_and_Viewer PRIVATE -Wall -Wextra -Wpedantic)
endif()
