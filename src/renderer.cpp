#include "renderer.hpp"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Add a global variable for the original position
static ImVec2 g_originalPos(10, 0);
static bool g_controlsMoved = false;
static bool g_firstFrame = true;

const float CONTROLS_WIDTH = 484.0f;
const float CONTROLS_HEIGHT = 350.0f;  // Increased from 275.0f
const float CONTROLS_MARGIN = 20.0f;

Renderer::Renderer(Window& window) : m_window(window) {
    initGL();
    initImGui();
    initPerformanceMonitoring();
    setupOrthographicView();
    
    m_scene = std::make_unique<Scene>();
    // Generate some random obstacles
    m_scene->generateRandomObstacles(10);
}

Renderer::~Renderer() {
    // Clean up performance monitoring
    glDeleteQueries(1, &m_gpuQuery);
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::checkGLError(const char* operation) {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::string errorStr;
        switch (error) {
            case GL_INVALID_ENUM: errorStr = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE: errorStr = "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: errorStr = "GL_INVALID_OPERATION"; break;
            case GL_OUT_OF_MEMORY: errorStr = "GL_OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: errorStr = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            default: errorStr = "UNKNOWN"; break;
        }
        std::cerr << "OpenGL error after " << operation << ": " << errorStr << " (0x"
                  << std::hex << error << std::dec << ")" << std::endl;
    }
}

void APIENTRY Renderer::messageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    const char* severityStr;
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: severityStr = "HIGH"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: severityStr = "MEDIUM"; break;
        case GL_DEBUG_SEVERITY_LOW: severityStr = "LOW"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severityStr = "NOTIFICATION"; break;
        default: severityStr = "UNKNOWN"; break;
    }

    const char* typeStr;
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: typeStr = "ERROR"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "DEPRECATED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeStr = "UNDEFINED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_PORTABILITY: typeStr = "PORTABILITY"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: typeStr = "PERFORMANCE"; break;
        case GL_DEBUG_TYPE_MARKER: typeStr = "MARKER"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: typeStr = "PUSH_GROUP"; break;
        case GL_DEBUG_TYPE_POP_GROUP: typeStr = "POP_GROUP"; break;
        case GL_DEBUG_TYPE_OTHER: typeStr = "OTHER"; break;
        default: typeStr = "UNKNOWN"; break;
    }

    // Only print medium and high severity messages
    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM) {
        std::cerr << "GL CALLBACK: " << severityStr << " severity, type = " << typeStr
                  << "\n   Message: " << message << std::endl;
    }
}

void Renderer::updateDPIScale() {
    int windowWidth, windowHeight;
    int framebufferWidth, framebufferHeight;
    
    glfwGetWindowSize(m_window.getHandle(), &windowWidth, &windowHeight);
    glfwGetFramebufferSize(m_window.getHandle(), &framebufferWidth, &framebufferHeight);
    
    m_dpiScaleX = static_cast<float>(framebufferWidth) / windowWidth;
    m_dpiScaleY = static_cast<float>(framebufferHeight) / windowHeight;
}

void Renderer::initGL() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Initialize DPI scaling
    updateDPIScale();

    // Set viewport
    int width, height;
    glfwGetFramebufferSize(m_window.getHandle(), &width, &height);
    glViewport(0, 0, width, height);

    // Enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(messageCallback, nullptr);
    
    // Filter messages
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    
    checkGLError("initGL");

    // Set up window resize callback
    glfwSetWindowSizeCallback(m_window.getHandle(), [](GLFWwindow* window, int width, int height) {
        // Get the Renderer instance from GLFW user pointer
        auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
        if (renderer) {
            renderer->handleWindowResize();
        }
    });

    // Set up framebuffer size callback
    glfwSetFramebufferSizeCallback(m_window.getHandle(), [](GLFWwindow* window, int width, int height) {
        auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
        if (renderer) {
            renderer->handleWindowResize();
        }
    });

    // Store this instance in GLFW window user pointer
    glfwSetWindowUserPointer(m_window.getHandle(), this);
}

void Renderer::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    // Increase default font size
    io.FontGlobalScale = 1.2f;

    // Apply custom styling
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Set color palette
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.133f, 0.133f, 0.133f, 1.00f); // #222222
    colors[ImGuiCol_Border] = ImVec4(0.114f, 0.600f, 0.624f, 1.00f);   // #1DCD9F
    colors[ImGuiCol_FrameBg] = ImVec4(0.133f, 0.133f, 0.133f, 1.00f);  // #222222
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.086f, 0.600f, 0.463f, 1.00f); // #169976
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.114f, 0.800f, 0.624f, 1.00f);  // #1DCD9F
    colors[ImGuiCol_TitleBg] = ImVec4(0.133f, 0.133f, 0.133f, 1.00f);  // #222222
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.133f, 0.133f, 0.133f, 1.00f); // #222222
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.133f, 0.133f, 0.133f, 1.00f); // #222222
    colors[ImGuiCol_Button] = ImVec4(0.114f, 0.800f, 0.624f, 1.00f);   // #1DCD9F
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.086f, 0.600f, 0.463f, 1.00f); // #169976
    colors[ImGuiCol_ButtonActive] = ImVec4(0.114f, 0.800f, 0.624f, 1.00f);  // #1DCD9F
    colors[ImGuiCol_Header] = ImVec4(0.114f, 0.800f, 0.624f, 1.00f);   // #1DCD9F
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.086f, 0.600f, 0.463f, 1.00f); // #169976
    colors[ImGuiCol_HeaderActive] = ImVec4(0.114f, 0.800f, 0.624f, 1.00f);  // #1DCD9F
    colors[ImGuiCol_SliderGrab] = ImVec4(0.114f, 0.800f, 0.624f, 1.00f);   // #1DCD9F
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.086f, 0.600f, 0.463f, 1.00f); // #169976
    colors[ImGuiCol_CheckMark] = ImVec4(0.114f, 0.800f, 0.624f, 1.00f);   // #1DCD9F
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);         // White text
    
    // Style settings
    style.WindowRounding = 5.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 3.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.TabRounding = 4.0f;
    
    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.FramePadding = ImVec2(5.0f, 3.0f);
    style.ItemSpacing = ImVec2(6.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
    style.IndentSpacing = 21.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 10.0f;
    
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;
    
    // Set background color for the main window
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // #000000

    ImGui_ImplGlfw_InitForOpenGL(m_window.getHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void Renderer::setupOrthographicView() {
    // Use framebuffer size for projection matrix
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(m_window.getHandle(), &fbWidth, &fbHeight);
    
    float width = static_cast<float>(fbWidth);
    float height = static_cast<float>(fbHeight);
    m_projectionMatrix = glm::ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -1.0f, 1.0f);
}

glm::vec2 Renderer::screenToWorld(double xpos, double ypos) {
    // Get framebuffer size for proper scaling
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(m_window.getHandle(), &fbWidth, &fbHeight);
    
    // Apply DPI scaling to input coordinates
    xpos *= m_dpiScaleX;
    ypos *= m_dpiScaleY;
    
    // Convert screen coordinates to world coordinates
    return glm::vec2(
        (float)xpos - (float)fbWidth/2.0f,
        (float)fbHeight/2.0f - (float)ypos  // Flip Y coordinate
    );
}

void Renderer::handleInput() {
    if (ImGui::GetIO().WantCaptureMouse) {
        return;  // Don't handle mouse input when ImGui is using it
    }

    double xpos, ypos;
    glfwGetCursorPos(m_window.getHandle(), &xpos, &ypos);
    glm::vec2 worldPos = screenToWorld(xpos, ypos);

    if (glfwGetMouseButton(m_window.getHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        m_scene->handleMousePress(worldPos);
    } else if (glfwGetMouseButton(m_window.getHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        m_scene->handleMouseRelease();
    }

    m_scene->handleMouseMove(worldPos);
}

void Renderer::handleWindowResize() {
    // Update DPI scaling
    updateDPIScale();
    
    // Get framebuffer size (accounts for DPI scaling)
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(m_window.getHandle(), &fbWidth, &fbHeight);
    
    // Update viewport using framebuffer size
    glViewport(0, 0, fbWidth, fbHeight);
    
    // Update projection matrix
    setupOrthographicView();
    
    // Update scene objects with framebuffer dimensions
    if (m_scene) {
        m_scene->handleWindowResize(fbWidth, fbHeight);
    }
    
    // Update the original position of the controls window to bottom right
    g_originalPos.x = ImGui::GetIO().DisplaySize.x - 10;
    g_originalPos.y = ImGui::GetIO().DisplaySize.y - 10;
    
    // Force reposition of controls window on resize
    g_firstFrame = true;
}

void Renderer::beginFrame() {
    // Check if framebuffer was resized
    static int lastWidth = 0;
    static int lastHeight = 0;
    
    int currentWidth, currentHeight;
    glfwGetFramebufferSize(m_window.getHandle(), &currentWidth, &currentHeight);
    
    if (currentWidth != lastWidth || currentHeight != lastHeight) {
        handleWindowResize();
        lastWidth = currentWidth;
        lastHeight = currentHeight;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    handleInput();
    m_scene->update();
    m_scene->render(m_projectionMatrix);
    
    // Update performance stats at the end of the frame
    updatePerformanceStats();
}

void Renderer::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    m_window.swapBuffers();
}

void Renderer::renderUI() {
    // Get the main viewport
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    
    // Set up the about button in the top-right corner
    ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - 50, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(40, 40), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);  // Match controls window rounding
    
    if (ImGui::Begin("AboutButton", nullptr, 
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
        ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoBackground)) {
        
        // Use the same styling as the refresh scene button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.114f, 0.800f, 0.624f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.086f, 0.600f, 0.463f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.114f, 0.800f, 0.624f, 1.00f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);  // Match controls window rounding
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        
        // Use a larger font size for the icon
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::SetWindowFontScale(1.5f);
        
        if (ImGui::Button("i", ImVec2(40, 40))) {
            m_showAboutWindow = true;
        }
        
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopFont();
        
        m_aboutButtonHovered = ImGui::IsItemHovered();
        
        // Add tooltip and cursor change
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("About RayTracerNG");
            ImGui::EndTooltip();
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
        
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(2);
    }
    ImGui::End();
    ImGui::PopStyleVar(3);

    // Add borderless fullscreen toggle button
    ImGui::SetNextWindowPos(ImVec2(viewport->Size.x - 100, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(40, 40), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);  // Match controls window rounding
    
    if (ImGui::Begin("FullscreenButton", nullptr, 
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
        ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoBackground)) {
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.114f, 0.800f, 0.624f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.086f, 0.600f, 0.463f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.114f, 0.800f, 0.624f, 1.00f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);  // Match controls window rounding
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        
        // Use a larger font size for the icon
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::SetWindowFontScale(1.5f);
        const char* icon = m_window.isBorderlessFullscreen() ? u8"\u2752" : u8"\u26F6";
        if (ImGui::Button(m_window.isBorderlessFullscreen() ? "[]" : "[]", ImVec2(40, 40))) {
            m_window.toggleBorderlessFullscreen();
        }
        
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopFont();
        
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text(m_window.isBorderlessFullscreen() ? "Exit Fullscreen" : "Enter Fullscreen");
            ImGui::EndTooltip();
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
        
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(2);
    }
    ImGui::End();
    ImGui::PopStyleVar(3);

    // Show the about window if needed
    if (m_showAboutWindow) {
        showAboutWindow();
    }

    // Set window flags for a more polished look
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;  // Remove NoResize and AlwaysAutoResize
    
    // Update original position if window size changed
    if (g_originalPos.x != ImGui::GetIO().DisplaySize.x - 10 || 
        g_originalPos.y != ImGui::GetIO().DisplaySize.y - 10) {
        g_originalPos.x = ImGui::GetIO().DisplaySize.x - 10;
        g_originalPos.y = ImGui::GetIO().DisplaySize.y - 10;
    }
    
    // Position the controls window at the bottom-right corner
    if (g_firstFrame) {
        ImGui::SetNextWindowPos(g_originalPos, ImGuiCond_Always, ImVec2(1, 1));
        g_firstFrame = false;
    } else {
        ImGui::SetNextWindowPos(g_originalPos, ImGuiCond_FirstUseEver, ImVec2(1, 1));
    }
    
    // Add static variables to track window state and size
    static bool isMinimized = false;
    static ImVec2 originalSize(0, 0);
    static float minimizedHeight = 50.0f;
    
    // Set initial window size
    if (originalSize.x == 0) {
        ImGui::SetNextWindowSize(ImVec2(420, 665));  // Increased width and height to fit content
    }
    
    // Begin the controls window with a title
    if (ImGui::Begin("Controls", nullptr, window_flags | ImGuiWindowFlags_NoScrollbar)) {  // Added NoScrollbar flag
        // Store the original size when first opened
        if (originalSize.x == 0) {
            originalSize = ImGui::GetWindowSize();
        }
        
        // Set fixed size based on state
        if (isMinimized) {
            ImGui::SetWindowSize(ImVec2(originalSize.x, minimizedHeight));
        }
        
        // Add minimize button in the title bar
        ImGui::SameLine(ImGui::GetWindowWidth() - 30);
        if (ImGui::Button(isMinimized ? "+" : "-", ImVec2(20, 20))) {
            isMinimized = !isMinimized;
            if (!isMinimized) {
                ImGui::SetWindowSize(originalSize);
            }
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
        
        if (!isMinimized) {
            // Increase font size for the controls section
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
            ImGui::SetWindowFontScale(1.2f);
            
            // Performance section with custom styling
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
            ImGui::Text("Performance");
            ImGui::PopStyleColor();
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            
            // Display actual CPU and GPU usage
            ImGui::Text("CPU Usage: %.1f%%", m_cpuUsage);
            ImGui::Text("GPU Usage: %.1f%%", m_gpuUsage);
            
            ImGui::Separator();
            
            // Light Source Controls with custom styling
            if (const LightSource* light = m_scene->getLightSource()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
                ImGui::Text("Light Source");
                ImGui::PopStyleColor();
                ImGui::Text("Position: (%.1f, %.1f)",
                    light->getPosition().x, light->getPosition().y);
                    
                // Add auto-move toggle with improved styling
                ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.114f, 0.800f, 0.624f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
                bool autoMove = m_scene->isLightAutoMoving();
                if (ImGui::Checkbox("Auto Move Light", &autoMove)) {
                    m_scene->setLightAutoMove(autoMove);
                    std::cout << "Light auto-move: " << (autoMove ? "Enabled" : "Disabled") << std::endl;
                }
                ImGui::PopStyleVar(2);
                ImGui::PopStyleColor(4);
                
                // Add tooltip
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("Enable to make the light source move automatically");
                    ImGui::Text("Auto-move will disable if you manually drag the light");
                    ImGui::EndTooltip();
                }

                // Add reflections toggle with improved styling
                ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.114f, 0.800f, 0.624f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
                bool reflectionsEnabled = m_scene->areReflectionsEnabled();
                if (ImGui::Checkbox("Enable Reflections", &reflectionsEnabled)) {
                    m_scene->setReflectionsEnabled(reflectionsEnabled);
                    std::cout << "Reflections: " << (reflectionsEnabled ? "Enabled" : "Disabled") << std::endl;
                }
                ImGui::PopStyleVar(2);
                ImGui::PopStyleColor(4);
                
                // Add tooltip
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("Enable/disable ray reflections");
                    ImGui::Text("When disabled, rays will stop at the first intersection");
                    ImGui::EndTooltip();
                }
            }
            
            ImGui::Separator();
            
            // Main Object Controls with custom styling
            if (const MainObject* obj = m_scene->getMainObject()) {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Main Object");
                ImGui::Text("Position: (%.1f, %.1f)",
                    obj->getPosition().x, obj->getPosition().y);
                
                ImGui::Text("Color");
                glm::vec3 objColor = obj->getColor();
                float color[3] = {objColor.r, objColor.g, objColor.b};
                if (ImGui::ColorEdit3("##ObjectColor", color)) {
                    m_scene->getMainObject()->setColor(glm::vec3(color[0], color[1], color[2]));
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                }
            }
            
            ImGui::Separator();
            
            // Obstacle Count with custom styling
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Scene Statistics");
            ImGui::Text("Number of Obstacles: %d", m_scene->getObstacleCount());
            
            // Add Obstacle Count Slider
            int currentCount = Scene::getDesiredObstacleCount();
            float sliderWidth = ImGui::GetWindowWidth() * 0.4f; // Make slider 60% of window width
            ImGui::PushItemWidth(sliderWidth);
            if (ImGui::SliderInt("Desired Obstacle Count", &currentCount, 0, 50)) {
                Scene::setDesiredObstacleCount(currentCount);
                m_scene->generateRandomObstacles(currentCount);
            }
            ImGui::PopItemWidth();
            
            // Add Ray Count Slider
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Ray Tracing Quality");
            int currentRayCount = Scene::getRayCount();
            const char* rayCountLabels[] = { "90 (Low)", "180 (Medium)", "360 (High)", "720 (Ultra)" };
            int rayCountValues[] = { 90, 180, 360, 720 };
            int currentIndex = 0;
            for (int i = 0; i < 4; i++) {
                if (rayCountValues[i] == currentRayCount) {
                    currentIndex = i;
                    break;
                }
            }
            if (ImGui::SliderInt("Ray Count", &currentIndex, 0, 3, rayCountLabels[currentIndex])) {
                Scene::setRayCount(rayCountValues[currentIndex]);
                m_scene->getLightSource()->updateRays();
            }
            
            // Add Performance Disclaimer
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
            ImGui::TextWrapped("Warning: Higher ray counts may significantly impact performance, especially with many obstacles.");
            ImGui::PopStyleColor();
            
            // Add Refresh Button
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Scene Controls");
            if (ImGui::Button("Refresh Scene", ImVec2(150, 30))) {
                m_scene->generateRandomObstacles(m_scene->getObstacleCount());
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            }
            
            // Add disclaimer text with improved styling
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
            ImGui::TextWrapped("Note: Refreshing the scene will regenerate all obstacles with new random positions.");
            ImGui::PopStyleColor();
            
            // Reset font scale
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopFont();
        } else {
            // When minimized, just show a small indicator
            ImGui::Text("Controls (Click + to expand)");
        }
        
        // Get the current window position
        ImVec2 currentPos = ImGui::GetWindowPos();
        
        // End the controls window
        ImGui::End();
        
        // Check if the window is being dragged
        static bool wasDragging = false;
        
        // Check if the window is being dragged
        if (ImGui::IsMouseDragging(0) && ImGui::IsWindowHovered()) {
            wasDragging = true;
            g_controlsMoved = true;
        } else if (wasDragging && !ImGui::IsMouseDragging(0)) {
            wasDragging = false;
            
            // Check if the window is near the original position
            float distance = sqrt(pow(currentPos.x - g_originalPos.x, 2) + pow(currentPos.y - g_originalPos.y, 2));
            
            if (distance < 100.0f) {
                // Snap back to the original position
                g_controlsMoved = false;
                ImGui::SetNextWindowPos(g_originalPos, ImGuiCond_Always, ImVec2(1, 1));
                if (ImGui::Begin("Controls", nullptr, window_flags)) {
                    ImGui::End();
                }
            }
        }
        
        // If the window was moved and is not being dragged, check if it should snap back
        if (g_controlsMoved && !wasDragging) {
            // Get the current window position
            ImVec2 pos = ImGui::GetWindowPos();
            float distance = sqrt(pow(pos.x - g_originalPos.x, 2) + pow(pos.y - g_originalPos.y, 2));
            
            if (distance < 100.0f) {
                // Snap back to the original position
                ImGui::SetNextWindowPos(g_originalPos, ImGuiCond_Always, ImVec2(1, 1));
                if (ImGui::Begin("Controls", nullptr, window_flags)) {
                    ImGui::End();
                }
                g_controlsMoved = false;
            }
        }
        
        // Create a frame around the scene area with increased thickness and transparent background
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); // Transparent background
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.114f, 0.600f, 0.624f, 1.0f)); // Fully opaque border
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 15.0f); // Increased border thickness
        if (ImGui::Begin("SceneFrame", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                                               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                                               ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs)) {
            ImGui::End();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
    }
}

void Renderer::showAboutWindow() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 center = ImVec2(viewport->Size.x * 0.5f, viewport->Size.y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_Always);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);  // Match controls window rounding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.133f, 0.133f, 0.133f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.114f, 0.600f, 0.624f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    
    if (ImGui::Begin("About RayTracerNG", &m_showAboutWindow, 
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar)) {  // Added NoScrollbar flag
        
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::SetWindowFontScale(1.4f);  // Increased font scale
        
        // Center the title
        float titleWidth = ImGui::CalcTextSize("RayTracerNG").x;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - titleWidth) * 0.5f);
        ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "RayTracerNG");  // Match controls text color
        
        ImGui::SetWindowFontScale(1.2f);  // Increased font scale for content
        ImGui::PopFont();
        
        ImGui::Separator();
        ImGui::Spacing();
        
        // Project description with consistent styling
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));  // Match controls text color
        ImGui::TextWrapped("RayTracerNG is a high-performance, real-time 2D ray tracing application built with C++17 and modern OpenGL. It simulates realistic light behavior and reflections in an interactive scene, allowing users to manipulate light sources and obstacles dynamically.");
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // About you with consistent styling
        ImGui::TextWrapped("I am Nischaya Garg, a Computer Science Engineering student specializing in Artificial Intelligence and Machine Learning (Hons. IBM). This project showcases my expertise in C++17, OpenGL, GLFW, GLM, and ImGui through the development of a real-time 2D ray tracing engine, with a focus on performance, interactivity, and modern graphics programming.");
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Copyright - centered with consistent styling
        float copyrightWidth = ImGui::CalcTextSize("© 2025 Nischaya Garg").x;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - copyrightWidth) * 0.5f);
        ImGui::Text("© 2025 Nischaya Garg");
        
        float rightsWidth = ImGui::CalcTextSize("All rights reserved").x;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - rightsWidth) * 0.5f);
        ImGui::Text("All rights reserved");
        ImGui::PopStyleColor();
        
        // Close button at the bottom
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 40);
        ImGui::Separator();
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 100) * 0.5f);
        
        // Style the close button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.114f, 0.800f, 0.624f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.086f, 0.600f, 0.463f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.114f, 0.800f, 0.624f, 1.00f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);  // Match controls window rounding
        
        if (ImGui::Button("Close", ImVec2(100, 30))) {
            m_showAboutWindow = false;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
        
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
    }
    ImGui::End();
    
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(3);
}

void Renderer::initPerformanceMonitoring() {
    // Initialize CPU monitoring
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    m_numProcessors = sysInfo.dwNumberOfProcessors;
    
    m_processHandle = GetCurrentProcess();
    FILETIME dummy;
    GetProcessTimes(m_processHandle, &m_lastCPU, &dummy, &m_lastUserCPU, &m_lastKernelCPU);
    
    // Initialize GPU monitoring
    glGenQueries(1, &m_gpuQuery);
}

void Renderer::updatePerformanceStats() {
    // Update CPU usage
    FILETIME ftime, dummy, user, kernel;
    GetProcessTimes(m_processHandle, &ftime, &dummy, &user, &kernel);
    
    // Convert FILETIME to ULONGLONG
    ULONGLONG lastUser = ((ULONGLONG)m_lastUserCPU.dwHighDateTime << 32) | m_lastUserCPU.dwLowDateTime;
    ULONGLONG lastKernel = ((ULONGLONG)m_lastKernelCPU.dwHighDateTime << 32) | m_lastKernelCPU.dwLowDateTime;
    ULONGLONG lastCPU = ((ULONGLONG)m_lastCPU.dwHighDateTime << 32) | m_lastCPU.dwLowDateTime;
    
    ULONGLONG currentUser = ((ULONGLONG)user.dwHighDateTime << 32) | user.dwLowDateTime;
    ULONGLONG currentKernel = ((ULONGLONG)kernel.dwHighDateTime << 32) | kernel.dwLowDateTime;
    ULONGLONG currentCPU = ((ULONGLONG)ftime.dwHighDateTime << 32) | ftime.dwLowDateTime;
    
    // Calculate CPU usage
    ULONGLONG userDiff = currentUser - lastUser;
    ULONGLONG kernelDiff = currentKernel - lastKernel;
    ULONGLONG cpuDiff = currentCPU - lastCPU;
    
    if (cpuDiff > 0) {
        float percent = (float)(userDiff + kernelDiff) / (float)cpuDiff;
        percent /= m_numProcessors;
        m_cpuUsage = percent * 100.0f;
    }
    
    // Store current values for next update
    m_lastCPU = ftime;
    m_lastUserCPU = user;
    m_lastKernelCPU = kernel;
    
    // Update GPU usage
    if (!m_gpuQueryStarted) {
        glBeginQuery(GL_TIME_ELAPSED, m_gpuQuery);
        m_gpuQueryStarted = true;
    } else {
        glEndQuery(GL_TIME_ELAPSED);
        GLuint64 timeElapsed;
        glGetQueryObjectui64v(m_gpuQuery, GL_QUERY_RESULT, &timeElapsed);
        
        // Convert nanoseconds to milliseconds and calculate percentage
        // Assuming 16.67ms (60 FPS) as 100% GPU usage
        m_gpuUsage = (timeElapsed / 1000000.0f) / 16.67f * 100.0f;
        m_gpuUsage = std::min(m_gpuUsage, 100.0f); // Cap at 100%
        
        // Start new query
        glBeginQuery(GL_TIME_ELAPSED, m_gpuQuery);
    }
}