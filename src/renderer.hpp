#pragma once

#include "window.hpp"
#include "scene.hpp"
#include <glm/glm.hpp>
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <windows.h>
#include <psapi.h>

class Renderer {
public:
    Renderer(Window& window);
    ~Renderer();

    static void APIENTRY messageCallback(GLenum source, GLenum type, GLuint id,
        GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
    static void checkGLError(const char* operation);

    void beginFrame();
    void endFrame();
    void setupOrthographicView();
    void renderUI();
    void handleInput();
    void handleWindowResize();
    void showAboutWindow();

private:
    Window& m_window;
    glm::mat4 m_projectionMatrix;
    std::unique_ptr<Scene> m_scene;
    
    // About window state
    bool m_showAboutWindow = false;
    bool m_aboutButtonHovered = false;
    
    // DPI scaling factors
    float m_dpiScaleX = 1.0f;
    float m_dpiScaleY = 1.0f;
    
    // Performance monitoring
    GLuint m_gpuQuery;
    float m_cpuUsage = 0.0f;
    float m_gpuUsage = 0.0f;
    bool m_gpuQueryStarted = false;
    FILETIME m_lastCPU, m_lastSysCPU, m_lastUserCPU, m_lastKernelCPU;
    int m_numProcessors;
    HANDLE m_processHandle;
    
    void initImGui();
    void initGL();
    void updateDPIScale();
    void initPerformanceMonitoring();
    void updatePerformanceStats();
    
    glm::vec2 screenToWorld(double xpos, double ypos);
};