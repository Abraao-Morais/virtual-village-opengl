#include <iostream>
#include <cstdlib>
#include <array>
#include <string>
#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sky_sphere.h"
#include "shadow_map.h"
#include "shader.h"
#include "camera.h"
#include "ground.h"
#include "house.h"
#include "windmill.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(GLFW_KEY_W, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(GLFW_KEY_S, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(GLFW_KEY_A, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(GLFW_KEY_D, deltaTime);
}

// --------------------------------------------------
// Main
// --------------------------------------------------

int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar GLFW.\n";
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criar janela
    window = glfwCreateWindow(WIDTH, HEIGHT, "Virtual Village - OpenGL", nullptr, nullptr);

    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW.\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    // Inicializar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erro ao inicializar GLAD.\n";

        glfwDestroyWindow(window);
        glfwTerminate();

        return EXIT_FAILURE;
    }

    // Configurar callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Capturar o rato para controlar a câmara
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Configuração OpenGL
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);

    Shader shader(VILLAGE_SHADER_DIR "/basic.vert", VILLAGE_SHADER_DIR"/basic.frag");
    Shader depthShader(VILLAGE_SHADER_DIR "/shadow_depth.vert", VILLAGE_SHADER_DIR "/shadow_depth.frag");
    Shader skyShader(VILLAGE_SHADER_DIR "/sky_sphere.vert", VILLAGE_SHADER_DIR "/sky_sphere.frag");

    SkySphere sky;
    sky.create(100.0f, 64, 32);

    if (!sky.loadTexture("../assets/skybox/TychoSkymapII.t5_08192x04096.jpeg")){
        std::cerr << "Nao foi possivel carregar a textura do ceu.\n";
        sky.cleanup();
        glfwDestroyWindow(window);
        glfwTerminate();

        return EXIT_FAILURE;
    }

    ShadowMap shadowMap;

    Ground ground;

    House house1;
    House house2;
    House house3;
    House house4;

    Windmill windmill;

    shader.use();
    GLuint shaderProgram = shader.id();
    GLint modelLocation = glGetUniformLocation(shaderProgram, "model");
    GLint viewLocation = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLocation = glGetUniformLocation(shaderProgram,"projection");
    GLint colorLocation = glGetUniformLocation(shaderProgram, "groundColor");
    GLint lightSpaceLocation = glGetUniformLocation(shaderProgram, "lightSpaceMatrix");
    GLint shadowMapLocation = glGetUniformLocation(shaderProgram, "shadowMap");

    glUniform1i(shadowMapLocation, 0);

    glm::mat4 modelGround(1.0f);

    glm::mat4 modelHouse1 = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(-3.0f, 0.0f, 0.0f)
    );

    glm::mat4 modelHouse2 = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(-9.0f, 0.0f, 0.0f)
    );

    glm::mat4 modelHouse3 = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(3.0f, 0.0f, 0.0f)
    );

    glm::mat4 modelHouse4 = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(9.0f, 0.0f, 0.0f)
    );

    // Matrizes de transformação dos moinhos
    glm::mat4 modelWindmill = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(0.0f, 0.0f, -6.0f)
    );

    glm::mat4 rotorModel = glm::translate(
            modelWindmill,
            glm::vec3(0.0f, 7.0f, 2.6f)
        );

    float bladeAngle = 0.0f;
    const float rotationSpeed = glm::radians(7.0f);

    // Luz direcional.
    glm::vec3 lightPosition(-6.0f, 12.0f, 8.0f);
    glm::vec3 lightTarget(3.0f, 0.0f, 0.0f);
    glm::mat4 lightProjection = glm::ortho(-18.0f, 18.0f, -18.0f, 18.0f, 1.0f, 50.0f);
    glm::mat4 lightView = glm::lookAt(lightPosition, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        // Ciclo principal
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        bladeAngle += rotationSpeed * deltaTime;

        rotorModel = glm::translate(
            modelWindmill,
            glm::vec3(0.0f, 7.0f, 2.6f)
        );

        rotorModel = glm::rotate(
            rotorModel,
            bladeAngle,
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        glm::mat4 view = camera.getViewMatrix();

        float aspectRatio = 1.0f;

        if (screenHeight > 0)
            aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

         glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 300.0f);


        if (shadowMap.isValid()) {
            shadowMap.bindForWriting();

            depthShader.use();

            GLint depthModelLocation = glGetUniformLocation(depthShader.id(), "model");
            GLint depthLightSpaceLocation = glGetUniformLocation(depthShader.id(), "lightSpaceMatrix");
            glUniformMatrix4fv(depthLightSpaceLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

            glUniformMatrix4fv(depthModelLocation, 1, GL_FALSE, glm::value_ptr(modelGround));
            ground.draw();

            glUniformMatrix4fv(depthModelLocation, 1, GL_FALSE, glm::value_ptr(modelHouse1));
            house1.draw();

            glUniformMatrix4fv(depthModelLocation, 1, GL_FALSE, glm::value_ptr(modelHouse2));
            house2.draw();

            glUniformMatrix4fv(depthModelLocation, 1, GL_FALSE, glm::value_ptr(modelHouse3));
            house3.draw();

            glUniformMatrix4fv(depthModelLocation, 1, GL_FALSE, glm::value_ptr(modelHouse4));
            house4.draw();

            glUniformMatrix4fv(depthModelLocation, 1, GL_FALSE, glm::value_ptr(modelWindmill));
            windmill.draw();

            glUniformMatrix4fv(depthModelLocation, 1, GL_FALSE, glm::value_ptr(rotorModel));
            windmill.drawBlades();

            // Voltar ao framebuffer padrão.
            shadowMap.unbind();
        }

        glViewport(0, 0, screenWidth, screenHeight);
        glClearColor(0.8156f, 0.4705f, 0.2823f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);

        // A esfera e vista por dentro.
        glDisable(GL_CULL_FACE);
        skyShader.use();

        // Obter os identificadores dos uniforms.
        GLint skyViewLocation = glGetUniformLocation(skyShader.id(), "view");
        GLint skyProjectionLocation = glGetUniformLocation(skyShader.id(), "projection");
        GLint skyTextureLocation = glGetUniformLocation(skyShader.id(), "skyTexture");

        // Enviar as matrizes para o shader.
        glUniformMatrix4fv(skyViewLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(skyProjectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        // Vincular a textura do céu à unidade 0.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sky.texture);

        glUniform1i(skyTextureLocation, 0);

        // Desenhar a esfera.
        sky.draw();

        // Restaurar o estado de profundidade.
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        // O restante da cena usa o estado habitual.
        glDisable(GL_CULL_FACE);

        shader.use();

        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glUniformMatrix4fv(lightSpaceLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // Disponibilizar o mapa de sombras ao fragment shader.
        shadowMap.bindDepthTexture(0);

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelGround));
        glUniform3f(colorLocation, 0.3098f, 0.2352f, 0.1411f);
        ground.draw();

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelHouse1));
        house1.draw(colorLocation);

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelHouse2));
        house2.draw(colorLocation);

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelHouse3));
        house3.draw(colorLocation);

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelHouse4));
        house4.draw(colorLocation);

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelWindmill));
        windmill.draw(colorLocation);

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(rotorModel));
        windmill.drawBlades(colorLocation);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    sky.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}