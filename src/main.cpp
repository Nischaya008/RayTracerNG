#include "window.hpp"
#include "renderer.hpp"
#include <stdexcept>
#include <iostream>

int main() {
    try {
        Window window(1280, 720, "RayTracer NG");
        Renderer renderer(window);

        while (!window.shouldClose()) {
            renderer.beginFrame();
            renderer.renderUI();
            renderer.endFrame();
            window.pollEvents();
        }

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}