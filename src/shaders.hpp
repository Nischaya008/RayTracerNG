#pragma once
#include <string>

namespace Shaders {
    const std::string vertexShaderSource = R"(
        #version 450 core
        layout (location = 0) in vec2 aPos;
        uniform mat4 projection;
        uniform vec2 position;
        uniform float scale;

        void main() {
            vec2 worldPos = aPos * scale + position;
            gl_Position = projection * vec4(worldPos, 0.0, 1.0);
        }
    )";

    const std::string fragmentShaderSource = R"(
        #version 450 core
        uniform vec3 color;
        out vec4 FragColor;

        void main() {
            FragColor = vec4(color, 1.0);
        }
    )";
}