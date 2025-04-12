#include "scene.hpp"
#include "shaders.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <random>
#include <algorithm>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>

// Initialize static member
int Scene::s_desiredObstacleCount = 10;
int Scene::s_rayCount = 90;  // Initialize with default value

// Helper function to create circle vertices
std::vector<float> createCircleVertices(int segments = 32) {
    std::vector<float> vertices;
    // Center vertex
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    
    // Circle vertices
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159f * float(i) / float(segments);
        vertices.push_back(cos(angle));
        vertices.push_back(sin(angle));
    }
    return vertices;
}

// GameObject implementations
void setupCircleBuffer(unsigned int& VAO, unsigned int& VBO, const std::vector<float>& vertices) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// LightSource implementation
LightSource::LightSource(Scene* scene, const glm::vec2& position)
    : GameObject(scene, position, 20.0f), m_intensity(1.0f) {
    auto vertices = createCircleVertices();
    setupCircleBuffer(m_VAO, m_VBO, vertices);

    // Initialize rays
    m_rays.resize(NUM_RAYS);
    updateRays();

    // Setup ray buffer
    glGenVertexArrays(1, &m_rayVAO);
    glGenBuffers(1, &m_rayVBO);

    // Setup crosshair buffer
    glGenVertexArrays(1, &m_crosshairVAO);
    glGenBuffers(1, &m_crosshairVBO);

    // Create crosshair vertices
    std::vector<float> crosshairVertices = {
        // Horizontal line
        -CROSSHAIR_LENGTH, 0.0f,
         CROSSHAIR_LENGTH, 0.0f,
        // Vertical line
        0.0f, -CROSSHAIR_LENGTH,
        0.0f,  CROSSHAIR_LENGTH
    };

    glBindVertexArray(m_crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, crosshairVertices.size() * sizeof(float), crosshairVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

LightSource::~LightSource() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_rayVAO);
    glDeleteBuffers(1, &m_rayVBO);
    glDeleteVertexArrays(1, &m_crosshairVAO);
    glDeleteBuffers(1, &m_crosshairVBO);
}

void LightSource::updateRays() {
    // Clear existing rays
    m_rays.clear();
    m_reflectedRays.clear();
    
    // Get the current ray count from Scene
    int numRays = Scene::getRayCount();
    
    // Generate rays in a circle around the light source
    for (int i = 0; i < numRays; i++) {
        float angle = 2.0f * 3.14159f * float(i) / float(numRays);
        glm::vec2 direction(cos(angle), sin(angle));
        
        Ray ray;
        ray.origin = m_position;
        ray.direction = direction;
        ray.length = MAX_RAY_LENGTH;
        ray.color = glm::vec3(RAY_INTENSITY);
        
        m_rays.push_back(ray);
    }
}

void LightSource::processReflections(const Ray& incidentRay, const glm::vec2& intersectionPoint, 
                                   const glm::vec2& normal, const GameObject* hitObject) {
    if (incidentRay.reflectionCount >= Ray::MAX_REFLECTIONS) return;

    Ray reflectedRay = incidentRay;
    reflectedRay.origin = intersectionPoint;
    reflectedRay.direction = incidentRay.calculateReflection(normal);
    reflectedRay.isReflected = true;
    reflectedRay.reflectionCount = incidentRay.reflectionCount + 1;
    
    // Make each subsequent reflection progressively shorter
    float lengthFactor = REFLECTION_LENGTH_FACTOR * std::pow(0.7f, reflectedRay.reflectionCount - 1);
    reflectedRay.length = MAX_RAY_LENGTH * lengthFactor;
    
    reflectedRay.color = calculateReflectionColor(incidentRay.color, reflectedRay.reflectionCount);
    
    m_reflectedRays.push_back(reflectedRay);
}

glm::vec3 LightSource::calculateReflectionColor(const glm::vec3& baseColor, int reflectionCount) const {
    // Base colors for different ray types (in RGB format)
    const glm::vec3 PRIMARY_RAY_COLOR(1.0f, 0.95f, 0.4f);     // Bright warm yellow
    const glm::vec3 REFLECTED_RAY_COLOR(1.0f, 0.65f, 0.2f);   // Orange color for first reflection
    const glm::vec3 REREFLECTED_RAY_COLOR(1.0f, 0.3f, 0.1f);  // Red color for second reflection

    // Select base color based on reflection count
    glm::vec3 color;
    if (reflectionCount == 0) {
        color = PRIMARY_RAY_COLOR;
    } else if (reflectionCount == 1) {
        color = REFLECTED_RAY_COLOR;
    } else {
        color = REREFLECTED_RAY_COLOR;
    }

    // Apply a gentler intensity falloff for better visibility
    float intensityFactor;
    if (reflectionCount == 0) {
        intensityFactor = 1.0f;  // Full intensity for primary rays
    } else if (reflectionCount == 1) {
        intensityFactor = 1.0f;  // High intensity for first reflection (orange)
    } else {
        intensityFactor = 1.0f; // High intensity for second reflection (red)
    }
    
    color *= intensityFactor;

    return color;
}

void LightSource::renderRays(const glm::mat4& projection, unsigned int shaderProgram) {
    // Update ray positions
    updateRays();
    
    // Prepare ray vertices
    std::vector<float> rayVertices;
    rayVertices.reserve((m_rays.size() + m_reflectedRays.size()) * 4); // 2 points per ray, 2 floats per point
    
    Scene* scene = static_cast<Scene*>(m_scene);
    
    // Process primary rays
    for (const auto& ray : m_rays) {
        // Ray start point
        rayVertices.push_back(ray.origin.x);
        rayVertices.push_back(ray.origin.y);
        
        // Check intersections with all objects
        float minDist = ray.length;
        float intersectionDist;
        glm::vec2 intersectionPoint;
        const GameObject* hitObject = nullptr;
        
        // Check main object
        if (ray.intersectsCircle(scene->getMainObject()->getPosition(),
                               scene->getMainObject()->getRadius(),
                               intersectionDist)) {
            minDist = intersectionDist;
            hitObject = scene->getMainObject();
            intersectionPoint = ray.origin + ray.direction * intersectionDist;
        }
        
        // Check obstacles
        for (const auto& obstacle : scene->getObstacles()) {
            if (ray.intersectsCircle(obstacle->getPosition(),
                                   obstacle->getRadius(),
                                   intersectionDist)) {
                if (intersectionDist < minDist) {
                    minDist = intersectionDist;
                    hitObject = obstacle.get();
                    intersectionPoint = ray.origin + ray.direction * intersectionDist;
                }
            }
        }
        
        // Ray end point
        glm::vec2 endPoint = ray.origin + ray.direction * minDist;
        rayVertices.push_back(endPoint.x);
        rayVertices.push_back(endPoint.y);
        
        // Process reflection if there was a hit and reflections are enabled
        if (hitObject && scene->areReflectionsEnabled()) {
            glm::vec2 normal = glm::normalize(intersectionPoint - hitObject->getPosition());
            processReflections(ray, intersectionPoint, normal, hitObject);
        }
    }
    
    // Process reflected rays and their subsequent reflections only if reflections are enabled
    if (scene->areReflectionsEnabled()) {
        std::vector<Ray> allReflectedRays = m_reflectedRays;
        m_reflectedRays.clear(); // Clear the original reflected rays as we'll process them all
        
        while (!allReflectedRays.empty()) {
            Ray currentRay = allReflectedRays.back();
            allReflectedRays.pop_back();
            
            // Add the current ray to the final reflected rays list
            m_reflectedRays.push_back(currentRay);
            
            // Add vertices for the current ray
            rayVertices.push_back(currentRay.origin.x);
            rayVertices.push_back(currentRay.origin.y);
            
            // Check intersections with all objects
            float minDist = currentRay.length;
            float intersectionDist;
            glm::vec2 intersectionPoint;
            const GameObject* hitObject = nullptr;
            
            // Check main object
            if (currentRay.intersectsCircle(scene->getMainObject()->getPosition(),
                                          scene->getMainObject()->getRadius(),
                                          intersectionDist)) {
                minDist = intersectionDist;
                hitObject = scene->getMainObject();
                intersectionPoint = currentRay.origin + currentRay.direction * intersectionDist;
            }
            
            // Check obstacles
            for (const auto& obstacle : scene->getObstacles()) {
                if (currentRay.intersectsCircle(obstacle->getPosition(),
                                              obstacle->getRadius(),
                                              intersectionDist)) {
                    if (intersectionDist < minDist) {
                        minDist = intersectionDist;
                        hitObject = obstacle.get();
                        intersectionPoint = currentRay.origin + currentRay.direction * intersectionDist;
                    }
                }
            }
            
            // Ray end point
            glm::vec2 endPoint = currentRay.origin + currentRay.direction * minDist;
            rayVertices.push_back(endPoint.x);
            rayVertices.push_back(endPoint.y);
            
            // Process further reflections if there was a hit and we haven't reached max reflections
            if (hitObject && currentRay.reflectionCount < Ray::MAX_REFLECTIONS) {
                glm::vec2 normal = glm::normalize(intersectionPoint - hitObject->getPosition());
                
                // Create the next reflection
                Ray nextReflection = currentRay;
                nextReflection.origin = intersectionPoint;
                nextReflection.direction = currentRay.calculateReflection(normal);
                nextReflection.isReflected = true;
                nextReflection.reflectionCount = currentRay.reflectionCount + 1;
                
                // Make each subsequent reflection progressively shorter
                float lengthFactor = REFLECTION_LENGTH_FACTOR * std::pow(0.7f, nextReflection.reflectionCount - 1);
                nextReflection.length = MAX_RAY_LENGTH * lengthFactor;
                
                nextReflection.color = calculateReflectionColor(currentRay.color, nextReflection.reflectionCount);
                
                // Add to the list of rays to process
                allReflectedRays.push_back(nextReflection);
            }
        }
    }
    
    // Update buffer data
    glBindVertexArray(m_rayVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_rayVBO);
    glBufferData(GL_ARRAY_BUFFER, rayVertices.size() * sizeof(float), rayVertices.data(), GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Render rays
    glUseProgram(shaderProgram);
    
    // Render primary rays
    glm::vec3 rayColor = m_color * RAY_INTENSITY;
    static_cast<Scene*>(m_scene)->setShaderUniforms(projection, glm::vec2(0.0f), 1.0f, rayColor);
    glLineWidth(1.5f);
    glDrawArrays(GL_LINES, 0, m_rays.size() * 2);
    
    // Render reflected rays with different colors and dashed lines only if reflections are enabled
    if (scene->areReflectionsEnabled()) {
        for (size_t i = 0; i < m_reflectedRays.size(); ++i) {
            const auto& ray = m_reflectedRays[i];
            static_cast<Scene*>(m_scene)->setShaderUniforms(projection, glm::vec2(0.0f), 1.0f, ray.color);
            
            // Draw dashed line manually by drawing multiple small segments
            const int numSegments = 10;
            const float segmentLength = 5.0f;
            const float gapLength = 5.0f;
            
            glm::vec2 start = ray.origin;
            glm::vec2 end = ray.origin + ray.direction * ray.length;
            glm::vec2 dir = glm::normalize(end - start);
            float totalLength = glm::length(end - start);
            float currentLength = 0.0f;
            
            while (currentLength < totalLength) {
                float remainingLength = totalLength - currentLength;
                float currentSegmentLength = std::min(segmentLength, remainingLength);
                
                glm::vec2 segmentStart = start + dir * currentLength;
                glm::vec2 segmentEnd = segmentStart + dir * currentSegmentLength;
                
                std::vector<float> segmentVertices = {
                    segmentStart.x, segmentStart.y,
                    segmentEnd.x, segmentEnd.y
                };
                
                glBindBuffer(GL_ARRAY_BUFFER, m_rayVBO);
                glBufferData(GL_ARRAY_BUFFER, segmentVertices.size() * sizeof(float), segmentVertices.data(), GL_DYNAMIC_DRAW);
                glDrawArrays(GL_LINES, 0, 2);
                
                currentLength += currentSegmentLength + gapLength;
            }
        }
    }
    
    glLineWidth(1.0f);
    glBindVertexArray(0);
}

void Scene::setShaderUniforms(const glm::mat4& projection, const glm::vec2& position, float scale, const glm::vec3& color) {
    glUniformMatrix4fv(m_uniforms.projection, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform2fv(m_uniforms.position, 1, glm::value_ptr(position));
    glUniform1f(m_uniforms.scale, scale);
    glUniform3fv(m_uniforms.color, 1, glm::value_ptr(color));
}

void LightSource::render(const glm::mat4& projection, unsigned int shaderProgram) {
    // Render the light source circle
    glUseProgram(shaderProgram);
    static_cast<Scene*>(m_scene)->setShaderUniforms(projection, m_position, m_radius, m_color);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 34); // Center vertex + 32 segments + 1 closing vertex

    // Render the rays
    renderRays(projection, shaderProgram);

    // Render the crosshair last, on top of everything
    renderCrosshair(projection, shaderProgram);
}

void LightSource::renderCrosshair(const glm::mat4& projection, unsigned int shaderProgram) {
    glUseProgram(shaderProgram);
    static_cast<Scene*>(m_scene)->setShaderUniforms(projection, m_position, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));

    glBindVertexArray(m_crosshairVAO);
    glLineWidth(CROSSHAIR_THICKNESS);
    glDrawArrays(GL_LINES, 0, 4);
    glLineWidth(1.0f);
    glBindVertexArray(0);
}

// Obstacle implementation
Obstacle::Obstacle(Scene* scene, const glm::vec2& position)
    : GameObject(scene, position, 30.0f) {
    auto vertices = createCircleVertices();
    setupCircleBuffer(m_VAO, m_VBO, vertices);
}

Obstacle::~Obstacle() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void Obstacle::render(const glm::mat4& projection, unsigned int shaderProgram) {
    glUseProgram(shaderProgram);
    static_cast<Scene*>(m_scene)->setShaderUniforms(projection, m_position, m_radius, m_color);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 34); // Center vertex + 32 segments + 1 closing vertex
}

// MainObject implementation
MainObject::MainObject(Scene* scene, const glm::vec2& position)
    : GameObject(scene, position, 25.0f), m_isDragging(false) {
    auto vertices = createCircleVertices();
    setupCircleBuffer(m_VAO, m_VBO, vertices);
}

MainObject::~MainObject() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void MainObject::render(const glm::mat4& projection, unsigned int shaderProgram) {
    glUseProgram(shaderProgram);
    static_cast<Scene*>(m_scene)->setShaderUniforms(projection, m_position, m_radius, m_color);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 34); // Center vertex + 32 segments + 1 closing vertex
}

// Scene implementation
Scene::Scene() {
    initShaders();
    
    // Initialize light source on the left side
    m_lightSource = std::make_unique<LightSource>(this, glm::vec2(-500.0f, 0.0f));
    
    // Set light source color to match our new color scheme
    m_lightSource->setColor(glm::vec3(1.0f, 0.95f, 0.4f));  // Bright warm yellow
    
    // Initialize main object at center with green color
    m_mainObject = std::make_unique<MainObject>(this, glm::vec2(0.0f, 0.0f));
    m_mainObject->setColor(glm::vec3(0.0f, 1.0f, 0.0f));
    
    m_draggedObject = nullptr;
    
    // Initialize light target position to current position
    m_lightTargetPos = m_lightSource->getPosition();
    
    // Generate obstacles using the desired count
    generateRandomObstacles(s_desiredObstacleCount);
}

Scene::~Scene() {
    glDeleteProgram(m_shaderProgram);
}

void Scene::cacheUniformLocations() {
    m_uniforms.projection = glGetUniformLocation(m_shaderProgram, "projection");
    m_uniforms.position = glGetUniformLocation(m_shaderProgram, "position");
    m_uniforms.scale = glGetUniformLocation(m_shaderProgram, "scale");
    m_uniforms.color = glGetUniformLocation(m_shaderProgram, "color");

    // Verify uniform locations
    if (m_uniforms.projection == -1 || m_uniforms.position == -1 ||
        m_uniforms.scale == -1 || m_uniforms.color == -1) {
        throw std::runtime_error("Failed to locate shader uniforms");
    }
}

void Scene::initShaders() {
    // Create vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexSource = Shaders::vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    // Check vertex shader compilation
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        throw std::runtime_error("Vertex shader compilation failed: " + std::string(infoLog));
    }

    // Create fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentSource = Shaders::fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // Check fragment shader compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        glDeleteShader(vertexShader);
        throw std::runtime_error("Fragment shader compilation failed: " + std::string(infoLog));
    }

    // Create shader program
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);

    // Check program linking
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        throw std::runtime_error("Shader program linking failed: " + std::string(infoLog));
    }

    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Cache uniform locations after program is linked
    cacheUniformLocations();
}

void Scene::update() {
    // Update game objects
    if (m_lightAutoMove && m_lightSource) {
        // Use delta time for frame-rate independent movement
        static float lastTime = 0.0f;
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        // Cap delta time to avoid large jumps
        deltaTime = std::min(deltaTime, 0.1f);
        
        m_lightAutoMoveTimer += deltaTime;
        
        // Get current light position
        glm::vec2 currentPos = m_lightSource->getPosition();
        
        // Check if we need a new target position
        bool needNewTarget = false;
        
        // Need new target if:
        // 1. Timer has expired
        // 2. We've reached the current target
        // 3. Current target is invalid
        if (m_lightAutoMoveTimer >= LIGHT_AUTO_MOVE_INTERVAL || 
            glm::length(currentPos - m_lightTargetPos) < 1.0f ||
            !checkValidPosition(m_lightTargetPos, m_lightSource->getRadius(), true)) {
            needNewTarget = true;
        }
        
        // Generate a new target position if needed
        if (needNewTarget) {
            m_lightAutoMoveTimer = 0.0f;
            
            // Calculate safe area bounds (excluding controls section)
            float controlsXMin = m_screenWidth/2.0f - CONTROLS_WIDTH - CONTROLS_MARGIN;
            float controlsYMin = -m_screenHeight/2.0f + CONTROLS_MARGIN;
            
            // Use screen bounds with padding, avoiding controls area
            float padding = m_lightSource->getRadius() + 30.0f;
            float minX = -m_screenWidth/2.0f + padding;
            float maxX = controlsXMin - padding;
            float minY = -m_screenHeight/2.0f + padding;
            float maxY = m_screenHeight/2.0f - padding;
            
            std::cout << "Screen bounds: (" << minX << ", " << minY << ") to (" << maxX << ", " << maxY << ")" << std::endl;
            
            // Try to find a valid position
            int maxAttempts = 300; // Increased from 200
            bool foundValidPosition = false;
            
            // Create a random number generator with a time-based seed
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_real_distribution<float> xDist(minX, maxX);
            static std::uniform_real_distribution<float> yDist(minY, maxY);
            
            // Try to find a position that's far from current position
            float minDistance = 400.0f; // Increased from 300.0f
            for (int i = 0; i < maxAttempts; i++) {
                // Generate a new position
                glm::vec2 newPos = glm::vec2(xDist(gen), yDist(gen));
                
                // Skip if too close to current position
                if (glm::length(newPos - currentPos) < minDistance) {
                    continue;
                }
                
                // Check if the position is valid
                bool isValid = true;
                
                // Check if position is in controls area
                if (newPos.x > controlsXMin && newPos.y > controlsYMin) {
                    isValid = false;
                    continue;
                }
                
                // Check distance to main object
                float distToMain = glm::length(newPos - m_mainObject->getPosition());
                if (distToMain < (m_lightSource->getRadius() + m_mainObject->getRadius() + 40.0f)) { // Increased from 30.0f
                    isValid = false;
                    continue;
                }
                
                // Check distance to obstacles
                for (const auto& obstacle : m_obstacles) {
                    float dist = glm::length(newPos - obstacle->getPosition());
                    if (dist < (m_lightSource->getRadius() + obstacle->getRadius() + 40.0f)) { // Increased from 30.0f
                        isValid = false;
                        break;
                    }
                }
                
                // Check screen bounds
                if (newPos.x < minX || newPos.x > maxX || newPos.y < minY || newPos.y > maxY) {
                    isValid = false;
                    continue;
                }
                
                if (isValid) {
                    m_lightTargetPos = newPos;
                    foundValidPosition = true;
                    std::cout << "Light auto-move: New target position: (" 
                              << m_lightTargetPos.x << ", " << m_lightTargetPos.y << ")" << std::endl;
                    break;
                }
            }
            
            // If no valid position found, try again with reduced minimum distance
            if (!foundValidPosition) {
                minDistance = 250.0f; // Increased from 150.0f
                for (int i = 0; i < maxAttempts; i++) {
                    glm::vec2 newPos = glm::vec2(xDist(gen), yDist(gen));
                    
                    if (glm::length(newPos - currentPos) < minDistance) {
                        continue;
                    }
                    
                    bool isValid = true;
                    if (newPos.x > controlsXMin && newPos.y > controlsYMin) {
                        isValid = false;
                        continue;
                    }
                    
                    float distToMain = glm::length(newPos - m_mainObject->getPosition());
                    if (distToMain < (m_lightSource->getRadius() + m_mainObject->getRadius() + 35.0f)) { // Increased from 30.0f
                        isValid = false;
                        continue;
                    }
                    
                    for (const auto& obstacle : m_obstacles) {
                        float dist = glm::length(newPos - obstacle->getPosition());
                        if (dist < (m_lightSource->getRadius() + obstacle->getRadius() + 35.0f)) { // Increased from 30.0f
                            isValid = false;
                            break;
                        }
                    }
                    
                    if (newPos.x < minX || newPos.x > maxX || newPos.y < minY || newPos.y > maxY) {
                        isValid = false;
                        continue;
                    }
                    
                    if (isValid) {
                        m_lightTargetPos = newPos;
                        foundValidPosition = true;
                        std::cout << "Light auto-move: New target position (reduced distance): (" 
                                  << m_lightTargetPos.x << ", " << m_lightTargetPos.y << ")" << std::endl;
                        break;
                    }
                }
            }
            
            // If still no valid position found, try with medium distance
            if (!foundValidPosition) {
                minDistance = 150.0f; // New medium distance threshold
                for (int i = 0; i < maxAttempts; i++) {
                    glm::vec2 newPos = glm::vec2(xDist(gen), yDist(gen));
                    
                    if (glm::length(newPos - currentPos) < minDistance) {
                        continue;
                    }
                    
                    bool isValid = true;
                    if (newPos.x > controlsXMin && newPos.y > controlsYMin) {
                        isValid = false;
                        continue;
                    }
                    
                    float distToMain = glm::length(newPos - m_mainObject->getPosition());
                    if (distToMain < (m_lightSource->getRadius() + m_mainObject->getRadius() + 30.0f)) {
                        isValid = false;
                        continue;
                    }
                    
                    for (const auto& obstacle : m_obstacles) {
                        float dist = glm::length(newPos - obstacle->getPosition());
                        if (dist < (m_lightSource->getRadius() + obstacle->getRadius() + 30.0f)) {
                            isValid = false;
                            break;
                        }
                    }
                    
                    if (newPos.x < minX || newPos.x > maxX || newPos.y < minY || newPos.y > maxY) {
                        isValid = false;
                        continue;
                    }
                    
                    if (isValid) {
                        m_lightTargetPos = newPos;
                        foundValidPosition = true;
                        std::cout << "Light auto-move: New target position (medium distance): (" 
                                  << m_lightTargetPos.x << ", " << m_lightTargetPos.y << ")" << std::endl;
                        break;
                    }
                }
            }
            
            // If still no valid position found, try with minimal distance
            if (!foundValidPosition) {
                minDistance = 75.0f; // Increased from 50.0f
                for (int i = 0; i < maxAttempts; i++) {
                    glm::vec2 newPos = glm::vec2(xDist(gen), yDist(gen));
                    
                    if (glm::length(newPos - currentPos) < minDistance) {
                        continue;
                    }
                    
                    bool isValid = true;
                    if (newPos.x > controlsXMin && newPos.y > controlsYMin) {
                        isValid = false;
                        continue;
                    }
                    
                    float distToMain = glm::length(newPos - m_mainObject->getPosition());
                    if (distToMain < (m_lightSource->getRadius() + m_mainObject->getRadius() + 25.0f)) {
                        isValid = false;
                        continue;
                    }
                    
                    for (const auto& obstacle : m_obstacles) {
                        float dist = glm::length(newPos - obstacle->getPosition());
                        if (dist < (m_lightSource->getRadius() + obstacle->getRadius() + 25.0f)) {
                            isValid = false;
                            break;
                        }
                    }
                    
                    if (newPos.x < minX || newPos.x > maxX || newPos.y < minY || newPos.y > maxY) {
                        isValid = false;
                        continue;
                    }
                    
                    if (isValid) {
                        m_lightTargetPos = newPos;
                        foundValidPosition = true;
                        std::cout << "Light auto-move: New target position (minimal distance): (" 
                                  << m_lightTargetPos.x << ", " << m_lightTargetPos.y << ")" << std::endl;
                        break;
                    }
                }
            }
            
            // Only reset to safe position if all attempts fail
            if (!foundValidPosition) {
                glm::vec2 safePos = findSafePosition();
                m_lightSource->setPosition(safePos);
                m_lightTargetPos = safePos;
                m_lightSource->updateRays();
                std::cout << "Light auto-move: All attempts failed, resetting to safe position: (" 
                          << safePos.x << ", " << safePos.y << ")" << std::endl;
                return;
            }
        }
        
        // Move towards target position
        glm::vec2 direction = m_lightTargetPos - currentPos;
        float distance = glm::length(direction);
        
        if (distance > 5.0f) {
            direction = glm::normalize(direction);
            
            // Make movement frame-rate independent
            float moveAmount = m_lightMoveSpeed * deltaTime;
            
            // Limit movement to avoid overshooting
            moveAmount = std::min(moveAmount, distance);
            
            glm::vec2 newPos = currentPos + direction * moveAmount;
            
            // Check if the new position is valid
            bool isValid = true;
            
            // Calculate controls area bounds
            float controlsXMin = m_screenWidth/2.0f - CONTROLS_WIDTH - CONTROLS_MARGIN;
            float controlsYMin = -m_screenHeight/2.0f + CONTROLS_MARGIN;
            
            // Check if position is in controls area
            if (newPos.x > controlsXMin && newPos.y > controlsYMin) {
                isValid = false;
            }
            
            // Check distance to main object
            float distToMain = glm::length(newPos - m_mainObject->getPosition());
            if (distToMain < (m_lightSource->getRadius() + m_mainObject->getRadius() + 20.0f)) {
                isValid = false;
            }
            
            // Check distance to obstacles
            for (const auto& obstacle : m_obstacles) {
                float dist = glm::length(newPos - obstacle->getPosition());
                if (dist < (m_lightSource->getRadius() + obstacle->getRadius() + 20.0f)) {
                    isValid = false;
                    break;
                }
            }
            
            // Check screen bounds
            float padding = m_lightSource->getRadius() + 20.0f;
            if (newPos.x < -m_screenWidth/2.0f + padding || newPos.x > controlsXMin - padding ||
                newPos.y < -m_screenHeight/2.0f + padding || newPos.y > m_screenHeight/2.0f - padding) {
                isValid = false;
            }
            
            if (isValid) {
                m_lightSource->setPosition(newPos);
                m_lightSource->updateRays();
            } else {
                // If new position is invalid, reset to a safe position
                glm::vec2 safePos = findSafePosition();
                m_lightSource->setPosition(safePos);
                m_lightTargetPos = safePos;
                m_lightSource->updateRays();
                std::cout << "Light auto-move: Invalid movement detected, reset to safe position: (" 
                          << safePos.x << ", " << safePos.y << ")" << std::endl;
            }
        }
    }
}

void Scene::render(const glm::mat4& projection) {
    m_lightSource->render(projection, m_shaderProgram);
    
    for (const auto& obstacle : m_obstacles) {
        obstacle->render(projection, m_shaderProgram);
    }
    
    m_mainObject->render(projection, m_shaderProgram);
}

GameObject* Scene::getClickedObject(const glm::vec2& mousePos) {
    // Check light source first
    float distToLight = glm::length(mousePos - m_lightSource->getPosition());
    if (distToLight < m_lightSource->getRadius()) {
        return m_lightSource.get();
    }
    
    // Then check main object
    float distToMain = glm::length(mousePos - m_mainObject->getPosition());
    if (distToMain < m_mainObject->getRadius()) {
        return m_mainObject.get();
    }
    
    return nullptr;
}

void Scene::handleMousePress(const glm::vec2& mousePos) {
    m_draggedObject = getClickedObject(mousePos);
    if (m_draggedObject) {
        m_draggedObject->setDragging(true);
        // Initialize both current and target positions to avoid initial jump
        m_currentMousePos = mousePos;
        m_targetMousePos = mousePos;
        
        // Disable auto-move if light source is manually dragged
        if (m_draggedObject == m_lightSource.get()) {
            m_lightAutoMove = false;
        }
    }
}

void Scene::handleMouseRelease() {
    if (m_draggedObject) {
        m_draggedObject->setDragging(false);
        m_draggedObject = nullptr;
    }
}

void Scene::handleMouseMove(const glm::vec2& mousePos) {
    // Update target mouse position
    m_targetMousePos = mousePos;
    
    // Smoothly interpolate current mouse position towards target
    m_currentMousePos = glm::mix(m_currentMousePos, m_targetMousePos, m_mouseLerpSpeed);
    
    if (m_draggedObject && m_draggedObject->isDragging()) {
        // Store old position in case we need to revert
        glm::vec2 oldPos = m_draggedObject->getPosition();
        
        // Use actual screen dimensions
        float screenWidth = m_screenWidth;
        float screenHeight = m_screenHeight;
        
        // Use interpolated mouse position for smoother movement
        glm::vec2 newPos = m_currentMousePos;
        
        // Clamp position to screen bounds with padding
        float padding = m_draggedObject->getRadius();
        glm::vec2 clampedPos = glm::vec2(
            glm::clamp(newPos.x, -screenWidth/2.0f + padding, screenWidth/2.0f - padding),
            glm::clamp(newPos.y, -screenHeight/2.0f + padding, screenHeight/2.0f - padding)
        );
        
        m_draggedObject->setPosition(clampedPos);
        
        // Update rays if the light source is being moved
        if (m_draggedObject == m_lightSource.get()) {
            m_lightSource->updateRays();
        }
        
        bool collision = false;
        for (const auto& obstacle : m_obstacles) {
            if (m_draggedObject->checkCollision(*obstacle)) {
                collision = true;
                break;
            }
        }
        
        // Also check collision between light source and main object
        if (!collision && m_draggedObject == m_lightSource.get()) {
            collision = m_lightSource->checkCollision(*m_mainObject);
        } else if (!collision && m_draggedObject == m_mainObject.get()) {
            collision = m_mainObject->checkCollision(*m_lightSource);
        }
        
        if (collision) {
            m_draggedObject->setPosition(oldPos);
        }
    }
}

bool Scene::checkValidPosition(const glm::vec2& position, float radius, bool isLightSource) const {
    // Define minimum safe distances
    const float MIN_DISTANCE_TO_LIGHT = isLightSource ? 20.0f : 100.0f;  // Much smaller distance for light source
    const float MIN_DISTANCE_TO_MAIN = isLightSource ? 20.0f : 80.0f;    // Much smaller distance for light source
    const float MIN_DISTANCE_TO_OBSTACLE = isLightSource ? 20.0f : 60.0f; // Much smaller distance for light source
    
    // Check collision with light source with minimum distance
    // Only check if the position is not the current light position
    if (position != m_lightSource->getPosition()) {
        float distToLight = glm::length(position - m_lightSource->getPosition());
        if (distToLight < (radius + m_lightSource->getRadius() + MIN_DISTANCE_TO_LIGHT)) {
            if (isLightSource) {
                std::cout << "Position invalid: Too close to light source (distance: " << distToLight 
                          << ", min required: " << (radius + m_lightSource->getRadius() + MIN_DISTANCE_TO_LIGHT) << ")" << std::endl;
            }
            return false;
        }
    }
    
    // Check collision with main object with minimum distance
    float distToMain = glm::length(position - m_mainObject->getPosition());
    if (distToMain < (radius + m_mainObject->getRadius() + MIN_DISTANCE_TO_MAIN)) {
        if (isLightSource) {
            std::cout << "Position invalid: Too close to main object (distance: " << distToMain 
                      << ", min required: " << (radius + m_mainObject->getRadius() + MIN_DISTANCE_TO_MAIN) << ")" << std::endl;
        }
        return false;
    }
    
    // Check collision with other obstacles with minimum distance
    for (const auto& obstacle : m_obstacles) {
        float dist = glm::length(position - obstacle->getPosition());
        if (dist < (radius + obstacle->getRadius() + MIN_DISTANCE_TO_OBSTACLE)) {
            if (isLightSource) {
                std::cout << "Position invalid: Too close to obstacle (distance: " << dist 
                          << ", min required: " << (radius + obstacle->getRadius() + MIN_DISTANCE_TO_OBSTACLE) << ")" << std::endl;
            }
            return false;
        }
    }
    
    // Check if position is within screen bounds with padding
    float padding = radius + (isLightSource ? 20.0f : 50.0f); // Much smaller padding for light source
    if (position.x < -m_screenWidth/2.0f + padding || position.x > m_screenWidth/2.0f - padding ||
        position.y < -m_screenHeight/2.0f + padding || position.y > m_screenHeight/2.0f - padding) {
        if (isLightSource) {
            std::cout << "Position invalid: Outside screen bounds (x: " << position.x 
                      << ", y: " << position.y << ", padding: " << padding << ")" << std::endl;
        }
        return false;
    }
    
    return true;
}

void Scene::handleWindowResize(int width, int height) {
    // Update bounds for obstacle placement
    m_screenWidth = static_cast<float>(width);
    m_screenHeight = static_cast<float>(height);
    
    // Ensure objects stay within new bounds
    if (m_lightSource) {
        glm::vec2 lightPos = m_lightSource->getPosition();
        lightPos.x = glm::clamp(lightPos.x, -m_screenWidth/2.0f + m_lightSource->getRadius(),
                               m_screenWidth/2.0f - m_lightSource->getRadius());
        lightPos.y = glm::clamp(lightPos.y, -m_screenHeight/2.0f + m_lightSource->getRadius(),
                               m_screenHeight/2.0f - m_lightSource->getRadius());
        m_lightSource->setPosition(lightPos);
    }
    
    if (m_mainObject) {
        glm::vec2 objPos = m_mainObject->getPosition();
        objPos.x = glm::clamp(objPos.x, -m_screenWidth/2.0f + m_mainObject->getRadius(),
                             m_screenWidth/2.0f - m_mainObject->getRadius());
        objPos.y = glm::clamp(objPos.y, -m_screenHeight/2.0f + m_mainObject->getRadius(),
                             m_screenHeight/2.0f - m_mainObject->getRadius());
        m_mainObject->setPosition(objPos);
    }
    
    // Ensure obstacles stay within new bounds
    for (auto& obstacle : m_obstacles) {
        glm::vec2 pos = obstacle->getPosition();
        pos.x = glm::clamp(pos.x, -m_screenWidth/2.0f + obstacle->getRadius(),
                          m_screenWidth/2.0f - obstacle->getRadius());
        pos.y = glm::clamp(pos.y, -m_screenHeight/2.0f + obstacle->getRadius(),
                          m_screenHeight/2.0f - obstacle->getRadius());
        obstacle->setPosition(pos);
    }
}

void Scene::generateRandomObstacles(int count) {
    // Clear existing obstacles
    m_obstacles.clear();
    
    // Use a static counter that increments each time the function is called
    static unsigned int seedCounter = 0;
    seedCounter++;
    
    // Create a deterministic but different seed each time
    unsigned int finalSeed = seedCounter * 2654435761u; // Large prime number for better distribution
    
    std::mt19937 gen(finalSeed);
    
    // Adjust distribution based on screen size
    float margin = 50.0f; // Keep obstacles away from edges
    
    // Define the controls section area to avoid (bottom right corner)
    float controlsWidth = 484.0f;  // Width of controls window
    float controlsHeight = 275.0f; // Height of controls window
    float controlsMargin = 20.0f;  // Additional margin around controls
    
    // Calculate the area to avoid
    float avoidXMin = m_screenWidth/2.0f - controlsWidth - controlsMargin;
    float avoidYMin = -m_screenHeight/2.0f + controlsMargin;
    
    // Create distributions that avoid the controls area
    std::uniform_real_distribution<float> xDist(-m_screenWidth/2.0f + margin, m_screenWidth/2.0f - margin);
    std::uniform_real_distribution<float> yDist(-m_screenHeight/2.0f + margin, m_screenHeight/2.0f - margin);
    std::uniform_real_distribution<float> grayDist(0.3f, 0.7f);
    
    int placedObstacles = 0;
    int maxAttempts = 1000; // Prevent infinite loop
    int attempts = 0;
    
    while (placedObstacles < count && attempts < maxAttempts) {
        glm::vec2 position(xDist(gen), yDist(gen));
        
        // Check if position is in the controls area
        bool inControlsArea = (position.x > avoidXMin && position.y > avoidYMin);
        
        // Check if position is valid (not overlapping with other objects and not in controls area)
        if (!inControlsArea && checkValidPosition(position, 30.0f, false)) {
            auto obstacle = std::make_unique<Obstacle>(this, position);
            // Set a random gray color
            float gray = grayDist(gen);
            obstacle->setColor(glm::vec3(gray, gray, gray));
            m_obstacles.push_back(std::move(obstacle));
            placedObstacles++;
        }
        attempts++;
    }
}

glm::vec2 Scene::findSafePosition() const {
    // Calculate safe area bounds (excluding controls section)
    float controlsXMin = m_screenWidth/2.0f - CONTROLS_WIDTH - CONTROLS_MARGIN;
    float controlsYMin = -m_screenHeight/2.0f + CONTROLS_MARGIN;
    
    // Use screen bounds with padding, avoiding controls area
    float padding = m_lightSource->getRadius() + 30.0f;
    float minX = -m_screenWidth/2.0f + padding;
    float maxX = controlsXMin - padding;
    float minY = -m_screenHeight/2.0f + padding;
    float maxY = m_screenHeight/2.0f - padding;
    
    // Create a random number generator
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> xDist(minX, maxX);
    static std::uniform_real_distribution<float> yDist(minY, maxY);
    
    // Try to find a safe position
    for (int i = 0; i < 100; i++) {
        glm::vec2 position(xDist(gen), yDist(gen));
        
        // Check if position is valid
        if (checkValidPosition(position, m_lightSource->getRadius(), true)) {
            return position;
        }
    }
    
    // If no safe position found, return a default position (top-left corner)
    return glm::vec2(minX + padding, maxY - padding);
}