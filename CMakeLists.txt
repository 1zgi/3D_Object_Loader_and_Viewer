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

add_executable(ViewMe
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

# Make the target a macOS bundle
set_target_properties(ViewMe PROPERTIES
        MACOSX_BUNDLE             TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER  "com.ctrlrings.viewme"
        MACOSX_BUNDLE_BUNDLE_NAME     "ViewMe"
        MACOSX_BUNDLE_SHORT_VERSION_STRING  "${vwmMac.v1}"
        MACOSX_BUNDLE_BUNDLE_VERSION       "${vwmMac.v1}"
)

# Embed the macOS icon
set(APP_ICON ${CMAKE_SOURCE_DIR}/ViewMeResources/ViewMe.icns)
if(EXISTS ${APP_ICON})
    set_source_files_properties(${APP_ICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(ViewMe PRIVATE ${APP_ICON})
    set_target_properties(ViewMe PROPERTIES
            MACOSX_BUNDLE_ICON_FILE "ViewMe.icns")
endif()

# Bundle shader files in the app
file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/src/Shaders/*.glsl")
foreach(SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
    set_source_files_properties(${SHADER_FILE} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources/shaders")
    target_sources(ViewMe PRIVATE ${SHADER_FILE})
endforeach()

# ---- tinyfiledialogs (for macOS/Linux Browse dialog)
target_sources(ViewMe PRIVATE external/tfd/tinyfiledialogs.c)
target_include_directories(ViewMe PRIVATE ${CMAKE_SOURCE_DIR}/external/tfd)

# Your header search paths
target_include_directories(ViewMe PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib/imgui
        ${CMAKE_SOURCE_DIR}/src/headers
)

# Link libs
target_link_libraries(ViewMe PRIVATE
        SDL2::SDL2
        OpenGL::GL
        glm::glm
)

# Link CoreFoundation on macOS for bundle resource path
if(APPLE)
    target_link_libraries(ViewMe PRIVATE "-framework CoreFoundation")
endif()

# ImGui OpenGL loader = GLAD (recommended)
target_compile_definitions(ViewMe PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

# Warnings
if(MSVC)
    target_compile_options(ViewMe PRIVATE /W4)
else()
    target_compile_options(ViewMe PRIVATE -Wall -Wextra -Wpedantic)
endif()
