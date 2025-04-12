#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <memory>

class Scene;  // Forward declaration

class GameObject {
public:
    GameObject(Scene* scene, const glm::vec2& position, float radius)
        : m_scene(scene), m_position(position), m_radius(radius), m_color(1.0f), m_isDragging(false) {}
    
    virtual ~GameObject() = default;
    
    virtual void update() {}
    virtual void render(const glm::mat4& projection, unsigned int shaderProgram) {}
    
    const glm::vec2& getPosition() const { return m_position; }
    void setPosition(const glm::vec2& position) { m_position = position; }
    float getRadius() const { return m_radius; }
    
    const glm::vec3& getColor() const { return m_color; }
    void setColor(const glm::vec3& color) { m_color = color; }
    
    bool isDragging() const { return m_isDragging; }
    void setDragging(bool dragging) { m_isDragging = dragging; }

    bool checkCollision(const GameObject& other) const {
        float distance = glm::length(m_position - other.getPosition());
        return distance < (m_radius + other.getRadius());
    }

protected:
    Scene* m_scene;
    glm::vec2 m_position;
    float m_radius;
    glm::vec3 m_color;
    bool m_isDragging;
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
};

struct Ray {
    glm::vec2 origin;
    glm::vec2 direction;
    float length;
    bool isReflected{false};
    int reflectionCount{0};
    glm::vec3 color{1.0f};
    static constexpr int MAX_REFLECTIONS = 3;
    
    // Helper function to check intersection with a circle
    bool intersectsCircle(const glm::vec2& center, float radius, float& intersectionDist) const {
        glm::vec2 toCircle = center - origin;
        float a = glm::dot(direction, direction);
        float b = -2.0f * glm::dot(toCircle, direction);
        float c = glm::dot(toCircle, toCircle) - radius * radius;
        
        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f) return false;
        
        float t = (-b - sqrt(discriminant)) / (2.0f * a);
        if (t < 0.0f) return false;
        
        intersectionDist = t;
        return t < length;
    }

    // Calculate reflection direction using Fresnel equations
    glm::vec2 calculateReflection(const glm::vec2& normal) const {
        // For 2D, we can use a simpler reflection formula
        return direction - 2.0f * glm::dot(direction, normal) * normal;
    }
};

class LightSource : public GameObject {
public:
    LightSource(Scene* scene, const glm::vec2& position);
    ~LightSource();
    
    float getIntensity() const { return m_intensity; }
    void setIntensity(float intensity) { m_intensity = intensity; }
    void render(const glm::mat4& projection, unsigned int shaderProgram) override;
    void updateRays();
    void renderRays(const glm::mat4& projection, unsigned int shaderProgram);
    void renderCrosshair(const glm::mat4& projection, unsigned int shaderProgram);

private:
    float m_intensity;
    
    // Ray configuration - Adjust these values to modify ray behavior
    // ============================================================
    
    // Number of rays to emit (higher = more detailed but slower)
    // Recommended range: 72 to 90 (1 ray per 5 degrees to 1 ray per 0.5 degrees)
    static constexpr int NUM_RAYS = 90;  // Default value, will be overridden by Scene::s_rayCount
    
    // Maximum length of rays (how far they can travel)
    // Increase for larger scenes, decrease for better performance
    static constexpr float MAX_RAY_LENGTH = 2000.0f;
    
    // Crosshair configuration
    static constexpr float CROSSHAIR_LENGTH = 10.0f;  // Reduced from 25.0f
    static constexpr float CROSSHAIR_THICKNESS = 2.0f;
    
    // Base intensity of rays (how bright they appear)
    // Range: 0.0f to 1.0f
    // Higher values = brighter rays
    static constexpr float RAY_INTENSITY = 0.9f;

    // Reflection configuration
    static constexpr float REFLECTION_INTENSITY_FACTOR = 0.7f;  // How much intensity is preserved after reflection
    static constexpr float REFLECTION_LENGTH_FACTOR = 0.05f;     // How much length is preserved after reflection (reduced from 0.5f)
    
    std::vector<Ray> m_rays;
    std::vector<Ray> m_reflectedRays;  // Store reflected rays separately
    unsigned int m_rayVAO = 0;
    unsigned int m_rayVBO = 0;
    unsigned int m_crosshairVAO = 0;
    unsigned int m_crosshairVBO = 0;

    // Helper methods for ray reflection
    void processReflections(const Ray& incidentRay, const glm::vec2& intersectionPoint, 
                          const glm::vec2& normal, const GameObject* hitObject);
    glm::vec3 calculateReflectionColor(const glm::vec3& baseColor, int reflectionCount) const;
};

class Obstacle : public GameObject {
public:
    Obstacle(Scene* scene, const glm::vec2& position);
    ~Obstacle();
    void render(const glm::mat4& projection, unsigned int shaderProgram) override;
};

class MainObject : public GameObject {
public:
    MainObject(Scene* scene, const glm::vec2& position);
    ~MainObject();
    
    bool isDragging() const { return m_isDragging; }
    void setDragging(bool dragging) { m_isDragging = dragging; }
    void render(const glm::mat4& projection, unsigned int shaderProgram) override;

private:
    bool m_isDragging;
};

class Scene {
public:
    Scene();
    ~Scene();
    
    // Control section dimensions
    static constexpr float CONTROLS_WIDTH{484.0f};
    static constexpr float CONTROLS_HEIGHT{275.0f};
    static constexpr float CONTROLS_MARGIN{20.0f};
    
    // Ray count control
    static int getRayCount() { return s_rayCount; }
    static void setRayCount(int count) { s_rayCount = count; }
    
    void update();
    void render(const glm::mat4& projection);
    
    void handleMousePress(const glm::vec2& mousePos);
    void handleMouseRelease();
    void handleMouseMove(const glm::vec2& mousePos);
    void handleWindowResize(int width, int height);
    
    void generateRandomObstacles(int count);
    int getObstacleCount() const { return m_obstacles.size(); }
    static int getDesiredObstacleCount() { return s_desiredObstacleCount; }
    static void setDesiredObstacleCount(int count) { s_desiredObstacleCount = count; }
    
    // Light source auto-move controls
    bool isLightAutoMoving() const { return m_lightAutoMove; }
    void setLightAutoMove(bool enabled) { m_lightAutoMove = enabled; }
    
    // Reflection controls
    bool areReflectionsEnabled() const { return m_reflectionsEnabled; }
    void setReflectionsEnabled(bool enabled) { m_reflectionsEnabled = enabled; }
    
    // Const access for reading
    const LightSource* getLightSource() const { return m_lightSource.get(); }
    const MainObject* getMainObject() const { return m_mainObject.get(); }
    
    // Non-const access for modification
    LightSource* getLightSource() { return m_lightSource.get(); }
    MainObject* getMainObject() { return m_mainObject.get(); }
    const std::vector<std::unique_ptr<Obstacle>>& getObstacles() const { return m_obstacles; }

    // Shader uniform helper
    void setShaderUniforms(const glm::mat4& projection, const glm::vec2& position,
                          float scale, const glm::vec3& color);

private:
    std::unique_ptr<LightSource> m_lightSource;
    std::unique_ptr<MainObject> m_mainObject;
    std::vector<std::unique_ptr<Obstacle>> m_obstacles;
    GameObject* m_draggedObject;
    glm::vec2 m_currentMousePos{0.0f};
    glm::vec2 m_targetMousePos{0.0f};
    float m_mouseLerpSpeed{0.8f}; // Adjust this for smoother/faster movement
    
    // Light source auto-move state
    bool m_lightAutoMove{false};
    float m_lightAutoMoveTimer{0.0f};
    static constexpr float LIGHT_AUTO_MOVE_INTERVAL{1.0f}; // Time between auto-moves in seconds (increased from 0.5f)
    glm::vec2 m_lightTargetPos{0.0f};
    float m_lightMoveSpeed{50.0f}; // Speed of light movement (increased from 15.0f)
    
    // Reflection state
    bool m_reflectionsEnabled{true};
    
    // Screen dimensions
    float m_screenWidth{1280.0f};
    float m_screenHeight{720.0f};
    
    // Static member for desired obstacle count
    static int s_desiredObstacleCount;
    
    // Static member for ray count
    static int s_rayCount;
    
    bool checkValidPosition(const glm::vec2& position, float radius, bool isLightSource = false) const;
    GameObject* getClickedObject(const glm::vec2& mousePos);
    
    // Add this new method declaration
    glm::vec2 findSafePosition() const;
    
    // Shader related members
    unsigned int m_shaderProgram;
    struct {
        GLint projection;
        GLint position;
        GLint scale;
        GLint color;
    } m_uniforms;
    
    void initShaders();
    void cacheUniformLocations();
};