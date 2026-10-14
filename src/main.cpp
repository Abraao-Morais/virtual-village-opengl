#include <iostream>
#include <cstdlib>
#include <array>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene/scene_transforms.h"
#include "shaders_utils.h"

#include "sky_sphere.h"
#include "shadow_map.h"
#include "shader.h"
#include "camera.h"
#include "ground.h"
#include "house.h"
#include "windmill.h"
#include "tower.h"

using SceneTransforms::createModel;

constexpr unsigned int WIDTH = 1000;
constexpr unsigned int HEIGHT = 750;

GLFWwindow* window = nullptr;
Camera camera;

int screenWidth = WIDTH;
int screenHeight = HEIGHT;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    screenWidth = width;
    screenHeight = height;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    camera.processMouse(xpos, ypos);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const int keys[] = {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D};

    for (int key : keys) {
        if (glfwGetKey(window, key) == GLFW_PRESS)
            camera.processKeyboard(key, deltaTime);
    }
}

void drawScene(GLint modelLocation, GLint colorLocation, const glm::mat4& modelGround, Ground& ground, House& house, const std::array<glm::mat4, 8>& houseModels,
    Windmill& windmill, const glm::mat4& modelWindmill, const glm::mat4& rotorModel, Tower& tower, const std::array<glm::mat4, 2>& towerModels) {

    ShaderUtils::setMat4(modelLocation, modelGround);
    ShaderUtils::setColor(colorLocation,glm::vec3(0.3098f, 0.2352f, 0.1411f));
    ground.draw();

    for (const auto& model : houseModels) {
        ShaderUtils::setMat4(modelLocation, model);
        house.draw(colorLocation);
    }

    ShaderUtils::setMat4(modelLocation, modelWindmill);
    windmill.draw(colorLocation);

    ShaderUtils::setMat4(modelLocation, rotorModel);
    windmill.drawBlades(colorLocation);

    for (const auto& model : towerModels) {
        ShaderUtils::setMat4(modelLocation, model);
        tower.draw(colorLocation);
    }
}

void drawShadowScene(GLint modelLocation, GLint lightSpaceLocation, const glm::mat4& lightSpaceMatrix, const glm::mat4& modelGround, Ground& ground,
    House& house, const std::array<glm::mat4, 8>& houseModels, Windmill& windmill, const glm::mat4& modelWindmill, const glm::mat4& rotorModel,
    Tower& tower, const std::array<glm::mat4, 2>& towerModels) {

    ShaderUtils::setMat4(lightSpaceLocation, lightSpaceMatrix);

    ShaderUtils::setMat4(modelLocation, modelGround);
    ground.draw();

    for (const auto& model : houseModels) {
        ShaderUtils::setMat4(modelLocation, model);
        house.draw();
    }

    ShaderUtils::setMat4(modelLocation, modelWindmill);
    windmill.draw();

    ShaderUtils::setMat4(modelLocation, rotorModel);
    windmill.drawBlades();

    for (const auto& model : towerModels) {
        ShaderUtils::setMat4(modelLocation, model);
        tower.draw();
    }
}

void drawSky(SkySphere& sky, Shader& skyShader, GLint viewLocation, GLint projectionLocation, GLint textureLocation, const glm::mat4& view, const glm::mat4& projection) {
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    skyShader.use();
    ShaderUtils::setMat4(viewLocation, view);
    ShaderUtils::setMat4(projectionLocation, projection);
    ShaderUtils::setInt(textureLocation, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sky.texture);

    sky.draw();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

void drawMinimap(Shader& shader, ShadowMap& shadowMap, Camera& camera, GLint modelLocation, GLint viewLocation, GLint projectionLocation, GLint colorLocation,
    GLint lightSpaceLocation, const glm::mat4& modelGround, const glm::mat4& lightSpaceMatrix, Ground& ground, House& house, const std::array<glm::mat4, 8>& houseModels,
    Windmill& windmill, const glm::mat4& modelWindmill, const glm::mat4& rotorModel, Tower& tower, const std::array<glm::mat4, 2>& towerModels) {
    
    if (screenWidth <= 0 || screenHeight <= 0) return;

    const int margin = 12;
    if (screenWidth <= 2 * margin || screenHeight <= 2 * margin) return;

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
    const glm::vec3 mapTarget(cameraPosition.x, 0.0f, cameraPosition.z);
    const glm::vec3 mapPosition(cameraPosition.x, 55.0f, cameraPosition.z);
    const glm::mat4 mapView = glm::lookAt(mapPosition, mapTarget, glm::vec3(0.0f, 0.0f, -1.0f));
    const float mapAspect = static_cast<float>(mapWidth) / static_cast<float>(mapHeight);
    const float halfHeight = 26.25f;
    const float halfWidth = halfHeight * mapAspect;
    const glm::mat4 mapProjection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.1f, 100.0f);

    shader.use();

    ShaderUtils::setMat4(viewLocation, mapView);
    ShaderUtils::setMat4(projectionLocation, mapProjection);
    ShaderUtils::setMat4(lightSpaceLocation, lightSpaceMatrix);
    shadowMap.bindDepthTexture(0);

    drawScene(modelLocation, colorLocation, modelGround, ground, house, houseModels, windmill, modelWindmill, rotorModel, tower, towerModels);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar GLFW.\n";
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Virtual Village - OpenGL", nullptr, nullptr);

    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW.\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erro ao inicializar GLAD.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);

    Shader shader(VILLAGE_SHADER_DIR "/basic.vert", VILLAGE_SHADER_DIR "/basic.frag");
    Shader depthShader(VILLAGE_SHADER_DIR "/shadow_depth.vert", VILLAGE_SHADER_DIR "/shadow_depth.frag");
    Shader skyShader(VILLAGE_SHADER_DIR "/sky_sphere.vert", VILLAGE_SHADER_DIR "/sky_sphere.frag");

    SkySphere sky;
    sky.create(100.0f, 64, 32);

    ShadowMap shadowMap;

    Ground ground;
    House house;
    Windmill windmill;
    Tower tower;

    const glm::mat4 modelGround(1.0f);

    const std::array<glm::mat4, 8> houseModels = {
        createModel({-9.0f, 0.0f, -4.0f}),
        createModel({-3.0f, 0.0f, -4.0f}),
        createModel({ 3.0f, 0.0f, -4.0f}),
        createModel({ 9.0f, 0.0f, -4.0f}),
        createModel({-9.0f, 0.0f, 4.0f}, 180.0f),
        createModel({-3.0f, 0.0f, 4.0f}, 180.0f),
        createModel({ 3.0f, 0.0f, 4.0f}, 180.0f),
        createModel({ 9.0f, 0.0f, 4.0f}, 180.0f)
    };

    const glm::mat4 modelWindmill = createModel({0.0f, 0.0f, -12.0f});

    const std::array<glm::mat4, 2> towerModels = {createModel({50.0f, 0.0f, -8.0f}), createModel({50.0f, 0.0f,  8.0f})};

    float bladeAngle = 0.0f;
    const float rotationSpeed = glm::radians(7.0f);
    const glm::vec3 lightPosition(-6.0f, 12.0f, 8.0f);
    const glm::vec3 lightTarget(3.0f, 0.0f, 0.0f);
    const glm::mat4 lightProjection = glm::ortho(-18.0f, 18.0f, -18.0f, 18.0f, 1.0f, 50.0f);
    const glm::mat4 lightView = glm::lookAt(lightPosition, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    const GLint modelLocation = glGetUniformLocation(shader.id(), "model");
    const GLint viewLocation = glGetUniformLocation(shader.id(), "view");
    const GLint projectionLocation = glGetUniformLocation(shader.id(), "projection");
    const GLint colorLocation = glGetUniformLocation(shader.id(), "groundColor");
    const GLint lightSpaceLocation = glGetUniformLocation(shader.id(), "lightSpaceMatrix");
    const GLint shadowMapLocation = glGetUniformLocation(shader.id(), "shadowMap");
    const GLint depthModelLocation = glGetUniformLocation(depthShader.id(), "model");
    const GLint depthLightSpaceLocation = glGetUniformLocation(depthShader.id(), "lightSpaceMatrix");
    const GLint skyViewLocation = glGetUniformLocation(skyShader.id(), "view");
    const GLint skyProjectionLocation = glGetUniformLocation(skyShader.id(), "projection");
    const GLint skyTextureLocation = glGetUniformLocation(skyShader.id(), "skyTexture");

    shader.use();
    ShaderUtils::setInt(shadowMapLocation, 0);

    while (!glfwWindowShouldClose(window)) {
        const float currentFrame = static_cast<float>(glfwGetTime());

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        bladeAngle += rotationSpeed * deltaTime;

        glm::mat4 rotorModel = glm::translate(modelWindmill, glm::vec3(0.0f, 7.0f, 2.6f));
        rotorModel = glm::rotate(rotorModel, bladeAngle, glm::vec3(0.0f, 0.0f, 1.0f));

        const glm::mat4 view = camera.getViewMatrix();
        const float aspectRatio = screenHeight > 0 ? static_cast<float>(screenWidth) / static_cast<float>(screenHeight) : 1.0f;
        const glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 300.0f);

        if (shadowMap.isValid()) {
            shadowMap.bindForWriting();
            depthShader.use();
            drawShadowScene(depthModelLocation, depthLightSpaceLocation, lightSpaceMatrix, modelGround, ground, house, houseModels, windmill, modelWindmill, rotorModel, tower, towerModels);
            shadowMap.unbind();
        }

        glViewport(0, 0, screenWidth, screenHeight);
        glClearColor(0.8156f, 0.4705f, 0.2823f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawSky(sky, skyShader, skyViewLocation, skyProjectionLocation, skyTextureLocation, view, projection);

        shader.use();

        ShaderUtils::setMat4(viewLocation, view);
        ShaderUtils::setMat4(projectionLocation, projection);
        ShaderUtils::setMat4(lightSpaceLocation, lightSpaceMatrix);
        shadowMap.bindDepthTexture(0);

        drawScene(modelLocation, colorLocation, modelGround, ground, house, houseModels, windmill, modelWindmill, rotorModel, tower, towerModels);
        drawMinimap(shader, shadowMap, camera, modelLocation, viewLocation, projectionLocation, colorLocation, lightSpaceLocation, modelGround,
            lightSpaceMatrix, ground, house, houseModels, windmill, modelWindmill, rotorModel, tower, towerModels);

        glViewport(0, 0, screenWidth, screenHeight);
        shader.use();

        ShaderUtils::setMat4(viewLocation, view);
        ShaderUtils::setMat4(projectionLocation, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    sky.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
