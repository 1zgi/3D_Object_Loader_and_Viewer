cmake_minimum_required(VERSION 3.20)
project(3D_Object_Loader_and_Viewer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

# OpenGL (system framework)
find_package(OpenGL REQUIRED)

# SDL2 (will be bundled for distribution)
find_package(SDL2 CONFIG REQUIRED)

# GLM (header-only) - use local include instead of Homebrew package
# find_package(glm CONFIG REQUIRED) # REMOVED - was causing dynamic linking
set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/glm)

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
        ${GLM_INCLUDE_DIR}  # GLM header-only include
)

# Link libs (GLM removed - header-only)
target_link_libraries(ViewMe PRIVATE
        SDL2::SDL2
        OpenGL::GL
        # glm::glm REMOVED - header-only, no linking needed
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

# macOS: Bundle SDL2 and fix library paths for standalone distribution
if(APPLE)
    # Create Frameworks directory and copy SDL2
    add_custom_command(TARGET ViewMe POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_BUNDLE_DIR:ViewMe>/Contents/Frameworks"
        COMMENT "Creating Frameworks directory"
    )
    
    # Copy SDL2 dylib to Frameworks
    add_custom_command(TARGET ViewMe POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                "$<TARGET_FILE:SDL2::SDL2>"
                "$<TARGET_BUNDLE_DIR:ViewMe>/Contents/Frameworks/"
        COMMENT "Copying SDL2 to app bundle"
    )
    
    # Fix SDL2 library path to use @executable_path
    add_custom_command(TARGET ViewMe POST_BUILD
        COMMAND install_name_tool -change 
                "/opt/homebrew/opt/sdl2/lib/libSDL2-2.0.0.dylib"
                "@executable_path/../Frameworks/libSDL2-2.0.0.dylib"
                "$<TARGET_FILE:ViewMe>"
        COMMENT "Fixing SDL2 library path"
    )
    
    # Add rpath for Frameworks directory
    add_custom_command(TARGET ViewMe POST_BUILD
        COMMAND install_name_tool -add_rpath 
                "@executable_path/../Frameworks"
                "$<TARGET_FILE:ViewMe>"
        COMMENT "Adding Frameworks rpath"
    )
    
    # Code sign the app (ad-hoc for development)
    add_custom_command(TARGET ViewMe POST_BUILD
        COMMAND codesign --force --deep --sign - 
                "$<TARGET_BUNDLE_DIR:ViewMe>"
        COMMENT "Code signing app bundle"
    )
endif()
