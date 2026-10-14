#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

#include "camera/camera.h"
#include "objects/sky_sphere.h"
#include "rendering/shadow_map.h"
#include "rendering/shader.h"
#include "scene/village_scene.h"

class Application {
    public:
        Application() = default;
        ~Application();

        bool initialize();
        void run();
        void shutdown();

    private:
        GLFWwindow* window = nullptr;

        Camera camera;
        SkySphere sky;

        std::unique_ptr<VillageScene> village;
        std::unique_ptr<ShadowMap> shadowMap;

        std::unique_ptr<Shader> shader;
        std::unique_ptr<Shader> depthShader;
        std::unique_ptr<Shader> skyShader;

        int screenWidth = 1000;
        int screenHeight = 750;

        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

        bool initialized = false;

        bool initializeWindow();
        bool initializeOpenGL();
        bool initializeResources();

        void processInput();
        void renderFrame();
        void renderMinimap(const glm::mat4& lightSpaceMatrix, GLint modelLocation, GLint viewLocation, GLint projectionLocation, GLint colorLocation, GLint lightSpaceLocation);
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
        static void mouseCallback(GLFWwindow* window, double xpos, double ypos);

        bool gWasPressed = false;
};
