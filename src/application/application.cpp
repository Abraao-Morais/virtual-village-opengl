#include "application/Application.h"

#include <iostream>
#include <cstdlib>
#include <array>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene/scene_transforms.h"
#include "rendering/shaders_utils.h"

#include <stdexcept>

#ifndef VILLAGE_SHADER_DIR
#error "VILLAGE_SHADER_DIR precisa estar definido no CMake."
#endif

namespace {
    constexpr int WINDOW_WIDTH = 1000;
    constexpr int WINDOW_HEIGHT = 750;

    const glm::vec3 GROUND_COLOR(0.3098f, 0.2352f, 0.1411f);

    const glm::vec3 LIGHT_POSITION(-20.0f, 35.0f, 25.0f);
    const glm::vec3 LIGHT_TARGET(3.0f, 0.0f, 0.0f);
}

Application::~Application() {
    shutdown();
}

bool Application::initialize() {
    if (!initializeWindow()) return false;

    if (!initializeOpenGL()) {
        shutdown();
        return false;
    }

    try {
        if (!initializeResources()) {
            shutdown();
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Erro ao inicializar recursos: " << e.what() << '\n';
        shutdown();
        return false;
    }

    initialized = true;
    return true;
}

bool Application::initializeWindow() {
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar GLFW.\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Virtual Village - OpenGL", nullptr, nullptr);

    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW.\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar o GLAD." << std::endl;
        return false;
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    screenWidth = WINDOW_WIDTH;
    screenHeight = WINDOW_HEIGHT;

    return true;
}

bool Application::initializeOpenGL(){
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Erro ao inicializar GLAD.\n";
        return false;
    }

    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);
    return true;
}

bool Application::initializeResources(){
    shader = std::make_unique<Shader>(VILLAGE_SHADER_DIR "/basic.vert", VILLAGE_SHADER_DIR "/basic.frag");
    depthShader = std::make_unique<Shader>(VILLAGE_SHADER_DIR "/shadow_depth.vert", VILLAGE_SHADER_DIR "/shadow_depth.frag");
    skyShader = std::make_unique<Shader>(VILLAGE_SHADER_DIR "/sky_sphere.vert", VILLAGE_SHADER_DIR "/sky_sphere.frag");

    village = std::make_unique<VillageScene>();
    shadowMap = std::make_unique<ShadowMap>();
    sky.create(100.0f, 64, 32);

    if (!sky.loadTexture("../assets/skybox/TychoSkymapII.t5_08192x04096.jpeg"))
        throw std::runtime_error("Falha ao carregar a textura do céu.");

    shader->use();

    const GLint shadowMapLocation = glGetUniformLocation(shader->id(), "shadowMap");
    ShaderUtils::setInt(shadowMapLocation, 0);
    return true;
}

void Application::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    const int keys[] = {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D};

    for (int key : keys) {
        if (glfwGetKey(window, key) == GLFW_PRESS)
            camera.processKeyboard(key, deltaTime);
    }

    const bool gIsPressed = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;

    if (gIsPressed && !gWasPressed)village->triggerGravestoneSwing();
    gWasPressed = gIsPressed;
}

void Application::run() {
    if (!initialized || !window) return;

    while (!glfwWindowShouldClose(window)) {
        const float currentFrame = static_cast<float>(glfwGetTime());

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput();
        village->update(deltaTime);
        renderFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Application::renderFrame() {
    if (screenWidth <= 0 || screenHeight <= 0) return;

    const glm::mat4 view = camera.getViewMatrix();
    const float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
    const glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 300.0f);
    const glm::mat4 lightProjection = glm::ortho(-55.0f, 55.0f, -35.0f, 35.0f, 1.0f, 100.0f);
    const glm::mat4 lightView = glm::lookAt(LIGHT_POSITION, LIGHT_TARGET, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    const GLint modelLocation = glGetUniformLocation(shader->id(), "model");
    const GLint viewLocation = glGetUniformLocation(shader->id(), "view");
    const GLint projectionLocation = glGetUniformLocation(shader->id(), "projection");
    const GLint colorLocation = glGetUniformLocation(shader->id(), "groundColor");
    const GLint lightSpaceLocation = glGetUniformLocation(shader->id(), "lightSpaceMatrix");
    const GLint depthModelLocation = glGetUniformLocation(depthShader->id(), "model");
    const GLint depthLightSpaceLocation = glGetUniformLocation(depthShader->id(), "lightSpaceMatrix");

    if (shadowMap->isValid()) {
        shadowMap->bindForWriting();
        depthShader->use();
        village->drawShadow(depthModelLocation, depthLightSpaceLocation, lightSpaceMatrix);
        shadowMap->unbind();
    }

    glViewport(0, 0, screenWidth, screenHeight);
    glClearColor(0.8156f, 0.4705f, 0.2823f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    skyShader->use();

    ShaderUtils::setMat4(glGetUniformLocation(skyShader->id(), "view"), view);
    ShaderUtils::setMat4(glGetUniformLocation(skyShader->id(), "projection"), projection);
    ShaderUtils::setInt(glGetUniformLocation(skyShader->id(), "skyTexture"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sky.texture);

    sky.draw();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);

    // Vila.
    shader->use();

    ShaderUtils::setMat4(viewLocation, view);
    ShaderUtils::setMat4(projectionLocation, projection);
    ShaderUtils::setMat4(lightSpaceLocation, lightSpaceMatrix);

    shadowMap->bindDepthTexture(0);

    village->draw(modelLocation, colorLocation);

    // Minimapa.
    renderMinimap(lightSpaceMatrix, modelLocation, viewLocation, projectionLocation, colorLocation, lightSpaceLocation);

    // Restaurar viewport e matrizes da câmera principal.
    glViewport(0, 0, screenWidth, screenHeight);
    shader->use();
    ShaderUtils::setMat4(viewLocation, view);
    ShaderUtils::setMat4(projectionLocation, projection);
}

void Application::renderMinimap(const glm::mat4& lightSpaceMatrix, GLint modelLocation, GLint viewLocation, GLint projectionLocation, GLint colorLocation, GLint lightSpaceLocation) {
    if (screenWidth <= 24 || screenHeight <= 24) return;

    constexpr int margin = 12;
    const int mapWidth = std::min(260, screenWidth - 2 * margin);
    const int mapHeight = std::min(195, screenHeight - 2 * margin);
    const int mapX = screenWidth - mapWidth - margin;
    const int mapY = screenHeight - mapHeight - margin;

    glEnable(GL_SCISSOR_TEST);
    glScissor(mapX, mapY, mapWidth, mapHeight);

    glClearColor(0.12f, 0.16f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_SCISSOR_TEST);
    glViewport(mapX, mapY, mapWidth, mapHeight);

    const glm::vec3 cameraPosition = camera.getPosition();
    const glm::vec3 target(cameraPosition.x, 0.0f, cameraPosition.z);
    const glm::vec3 position(cameraPosition.x, 55.0f, cameraPosition.z);
    const glm::mat4 mapView = glm::lookAt(position, target,glm::vec3(0.0f, 0.0f, -1.0f));
    const float mapAspect = static_cast<float>(mapWidth) / static_cast<float>(mapHeight);
    constexpr float halfHeight = 26.25f;
    const float halfWidth = halfHeight * mapAspect;
    const glm::mat4 mapProjection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.1f, 100.0f);

    shader->use();
    ShaderUtils::setMat4(viewLocation, mapView);
    ShaderUtils::setMat4(projectionLocation, mapProjection);
    ShaderUtils::setMat4(lightSpaceLocation, lightSpaceMatrix);

    shadowMap->bindDepthTexture(0);
    village->draw(modelLocation, colorLocation);
}

void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));

    if (!app) return;

    app->screenWidth = width;
    app->screenHeight = height;
    glViewport(0, 0, width, height);
}

void Application::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) app->camera.processMouse(xpos, ypos);
}

void Application::shutdown() {
    initialized = false;

    if (window) glfwMakeContextCurrent(window);

    sky.cleanup();

    village.reset();
    shadowMap.reset();

    shader.reset();
    depthShader.reset();
    skyShader.reset();

    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}