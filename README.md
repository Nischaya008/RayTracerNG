![RayTracerNG Banner 1](https://github.com/Nischaya008/RayTracerNG/blob/main/RayTracerNG_Assets/Screenshot%202025-04-12%20175423.png)
![RayTracerNG Banner 2](https://github.com/Nischaya008/RayTracerNG/blob/main/RayTracerNG_Assets/Screenshot%202025-04-13%20202525.png)

<p align="center">
  <img src="assets/banner1.png" alt="RayTracerNG Banner" width="100%" id="banner-slider">
</p>

<script>
  let bannerIndex = 0;
  const banners = ["assets/banner1.png", "assets/banner2.png"];
  setInterval(() => {
    bannerIndex = (bannerIndex + 1) % banners.length;
    document.getElementById("banner-slider").src = banners[bannerIndex];
  }, 5000);
</script>

# RayTracerNG - Modern 2D Ray Tracing Engine

## 🚀 Overview
RayTracerNG is a powerful, real-time 2D ray tracing engine designed using modern OpenGL and C++. It provides an interactive environment to explore light physics, offering dynamic lighting, interactive scene manipulation, and real-time performance metrics.

---

## 🧠 Features
- 🔦 Real-time 2D ray tracing with 360° light emission
- 🧩 Drag-and-drop scene editing with collision detection
- 💡 Dynamic light sources and configurable reflections
- 🎛️ ImGui-based control panel for real-time tweaks
- 📊 Built-in CPU/GPU performance monitoring
- 🔄 Save/load scenes, auto-generate obstacles
- ⚙️ Cross-platform compatibility (Windows/Linux)

---

## 🛠️ Technical Architecture
**Languages & Libraries:**
- C++17
- OpenGL 4.6
- GLFW (Window management)
- GLM (Math library)
- ImGui (UI framework)
- CMake (Build system)

**System Requirements:**
- Windows 10+
- OpenGL 4.6+ compatible GPU
- 4GB RAM, 500MB storage

---

## 🧩 Core Components
### 🔳 Window Management
- GLFW window creation
- Borderless fullscreen support
- OpenGL context provisioning

### 🎨 Renderer
- Modern OpenGL pipeline
- ImGui integration
- DPI scaling & debug callbacks

### 🌐 Scene Management
- Scene graph & object management
- Real-time ray calculations
- Collision & shader handling

### 🔦 LightSource
- 360° ray emission with reflection
- Crosshair and intensity control

### 🧱 Obstacle & MainObject
- Configurable circular obstacles
- Drag-and-drop object manipulation
- Collision-aware placement

### 🔁 Ray System
- 90 rays by default, 3 max reflections
- 2000.0f max length, color-coded rays

---

## 📘 User Guide
### 🧭 Basic Controls
- Drag light/object with left-click
- Delete object: right-click
- Zoom: mouse wheel (if enabled)
- Pan: middle mouse drag

### 🔧 Advanced Features
- Adjust ray count & reflection intensity
- Auto-generate random scenes
- Monitor FPS, CPU, GPU, and memory

---

## 👨‍💻 Development Guide
### 🔨 Build from Source
#### Requirements:
- CMake 3.15+
- Visual Studio 2019+

#### Commands:
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 📁 Code Structure
```
src/         # Core C++ files
frontend/    # UI components
assets/      # Textures, banners, icons
```

### ✨ Extend Functionality
- Add new objects via `GameObject` inheritance
- Modify shaders in `shaders/`
- Extend UI via ImGui

---

## ⚙️ Performance Considerations
- Ray optimization with spatial partitioning
- Early termination & batch processing
- Efficient shader use and memory pooling

---

## 📦 Installation
### 🪟 Windows
1. Download installer
2. Run and follow setup wizard
3. Launch via Start Menu or Desktop Shortcut

### 🧰 Manual
```bash
git clone https://github.com/Nischaya008/RayTracerNG
cd RayTracerNG
mkdir build && cd build
cmake .. && cmake --build .
```

---

## 📄 License
**This project is proprietary and strictly confidential. Unauthorized use, reproduction, distribution, or modification is strictly prohibited and will result in legal action. All rights reserved. See the [LICENSE](https://github.com/Nischaya008/ResumifyNG/blob/main/LICENSE) for details.**

---

## 🤝 Contributing
1. Fork the repo
2. Create a feature branch
3. Commit & push your changes
4. Submit a Pull Request

---

## 💬 Support
Open an issue or contact the maintainer via GitHub.
### ⭐ Star this repository if you find it useful! 😊
---

## 📞 Contact
For any inquiries or feedback, reach out via:
- 📧 Email: nischayagarg008@gmail.com
- 🐦 Twitter: [@Nischaya008](https://x.com/Nischaya008)
- 💼 LinkedIn: [Nischaya Garg](https://www.linkedin.com/in/nischaya008/)

> “Light is the shadow of God.” – Plato
Stay Innovated, Keep Coding, Think BIG! 🚀
