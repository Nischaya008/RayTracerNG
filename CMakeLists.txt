cmake_minimum_required(VERSION 3.15)
project(RayTracerNG VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable static linking
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Add dependencies
# Add GLFW
add_subdirectory(${CMAKE_SOURCE_DIR}/libs/glfw/glfw-3.3.8)

# GLM is header-only, just need to include it
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_SOURCE_DIR}/libs/glm/glm-0.9.9.8)

# ImGui source files
set(IMGUI_SOURCES
    ${CMAKE_SOURCE_DIR}/libs/imgui/imgui-docking/imgui.cpp
    ${CMAKE_SOURCE_DIR}/libs/imgui/imgui-docking/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/libs/imgui/imgui-docking/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/libs/imgui/imgui-docking/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/libs/imgui/imgui-docking/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/libs/imgui/imgui-docking/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/libs/imgui/imgui-docking/backends/imgui_impl_opengl3.cpp
)

# Main application source files
set(APP_SOURCES
    src/main.cpp
    src/window.cpp
    src/renderer.cpp
    src/scene.cpp
    ${IMGUI_SOURCES}
)

# Add GLAD
set(GLAD_SOURCES
    ${CMAKE_SOURCE_DIR}/libs/glad-gen/src/glad.c
)

# Create executable
add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
    ${GLAD_SOURCES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/libs/imgui/imgui-docking
    ${CMAKE_SOURCE_DIR}/libs/imgui/imgui-docking/backends
    ${CMAKE_SOURCE_DIR}/libs/glfw/glfw-3.3.8/include
    ${CMAKE_SOURCE_DIR}/libs/glm/glm-0.9.9.8
    ${CMAKE_SOURCE_DIR}/libs/glad-gen/include
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glm
    opengl32
    psapi
    kernel32
)

# Set static runtime for MSVC
if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Copy SDL3.dll to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/SDL3.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL3.dll
)