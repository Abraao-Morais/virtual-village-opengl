#include <iostream>
#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "ground.h"
#include "house.h"

// Dimensões iniciais da janela
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// Janela e câmara
GLFWwindow* window = nullptr;
Camera camera;

int screenWidth = WIDTH;
int screenHeight = HEIGHT;

// Controlo do tempo
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// --------------------------------------------------
// Callbacks
// --------------------------------------------------

void framebuffer_size_callback(
    GLFWwindow* window,
    int width,
    int height
) {
    screenWidth = width;
    screenHeight = height;

    glViewport(0, 0, width, height);
}

void mouse_callback(
    GLFWwindow* window,
    double xpos,
    double ypos
) {
    camera.processMouse(xpos, ypos);
}

// --------------------------------------------------
// Input
// --------------------------------------------------

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.processKeyboard(GLFW_KEY_W, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.processKeyboard(GLFW_KEY_S, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboard(GLFW_KEY_A, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboard(GLFW_KEY_D, deltaTime);
    }
}

// --------------------------------------------------
// Shaders
// --------------------------------------------------

GLuint createShaderProgram() {
    const char* vertexShaderSource = R"(
        #version 400 core

        layout (location = 0) in vec3 vertexPosition;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main() {
            gl_Position =
                projection * view * model *
                vec4(vertexPosition, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 400 core

        out vec4 FragColor;

        uniform vec3 groundColor;

        void main() {
            FragColor = vec4(groundColor, 1.0);
        }
    )";

    // Compilar vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Erro no vertex shader:\n"
                  << infoLog << '\n';
    }

    // Compilar fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Erro no fragment shader:\n"
                  << infoLog << '\n';
    }

    // Criar programa
    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Erro ao ligar shaders:\n"
                  << infoLog << '\n';
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
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
    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    // Criar janela
    window = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "Virtual Village - OpenGL",
        nullptr,
        nullptr
    );

    if (!window) {
        std::cerr << "Erro ao criar a janela GLFW.\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    // Inicializar GLAD
    if (!gladLoadGLLoader(
            (GLADloadproc)glfwGetProcAddress
        )) {
        std::cerr << "Erro ao inicializar GLAD.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwSetFramebufferSizeCallback(
        window,
        framebuffer_size_callback
    );

    glfwSetCursorPosCallback(window, mouse_callback);

    // Capturar o rato para controlar a câmara
    glfwSetInputMode(
        window,
        GLFW_CURSOR,
        GLFW_CURSOR_DISABLED
    );

    // Configuração OpenGL
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);

    // Criar shader
    GLuint shaderProgram = createShaderProgram();

    // Criar objetos da cena
    Ground ground;
    House house;

    // Ciclo principal
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(
            glfwGetTime()
        );

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Limpar o ecrã
        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );

        // Ativar shader
        glUseProgram(shaderProgram);

        // Matrizes da cena
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getViewMatrix();

        float aspectRatio = static_cast<float>(screenWidth) /
                            static_cast<float>(screenHeight);

        glm::mat4 projection = glm::perspective(
            glm::radians(60.0f),
            aspectRatio,
            0.1f,
            300.0f
        );

        // Enviar matrizes ao shader
        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "model"),
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "view"),
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "projection"),
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );

        // Desenhar terreno
        GLint colorLocation = glGetUniformLocation(
            shaderProgram,
            "groundColor"
        );

        glUniform3f(
            colorLocation,
            0.25f, 0.55f, 0.25f
        );

        ground.draw();

        // Desenhar casa
        house.draw(colorLocation);

        // Apresentar o frame
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Libertar recursos
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
