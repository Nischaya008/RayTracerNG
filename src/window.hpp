#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    GLFWwindow* getHandle() const { return m_window; }

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    void toggleBorderlessFullscreen();
    bool isBorderlessFullscreen() const { return m_isBorderlessFullscreen; }

private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    bool m_isBorderlessFullscreen = false;
    int m_savedX, m_savedY, m_savedWidth, m_savedHeight;

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void errorCallback(int error, const char* description);
};