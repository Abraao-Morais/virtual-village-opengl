#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "ground.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

GLFWwindow* Window = nullptr;

Camera camera;
int screenWidth = WIDTH;
int screenHeight = HEIGHT;

// --------------------------------------------------
// Callbacks
// --------------------------------------------------

void framebufferSizeCallback(
    GLFWwindow* window,
    int width,
    int height
) {
    screenWidth = width;
    screenHeight = height;

    glViewport(0, 0, width, height);
}

void mouseCallback(
    GLFWwindow* window,
    double xpos,
    double ypos
) {
    camera.processMouse(xpos, ypos);
}

// --------------------------------------------------
// Inicialização da janela e do OpenGL
// --------------------------------------------------

void inicializaOpenGL() {
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW.\n";
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR, 4
    );
    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR, 0
    );
    glfwWindowHint(
        GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE
    );

    Window = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "Virtual Village - OpenGL",
        nullptr,
        nullptr
    );

    if (!Window) {
        std::cerr << "Falha ao criar janela.\n";
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(Window);

    glfwSetFramebufferSizeCallback(
        Window, framebufferSizeCallback
    );

    // Captura o mouse para controlar a visão.
    glfwSetInputMode(
        Window,
        GLFW_CURSOR,
        GLFW_CURSOR_DISABLED
    );

    glfwSetCursorPosCallback(Window, mouseCallback);

    if (!gladLoadGLLoader(
        (GLADloadproc)glfwGetProcAddress
    )) {
        std::cerr << "Falha ao inicializar GLAD.\n";
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    // Habilita o teste de profundidade 3D.
    glEnable(GL_DEPTH_TEST);

    std::cout << "GPU: "
              << glGetString(GL_RENDERER) << '\n';

    std::cout << "OpenGL: "
              << glGetString(GL_VERSION) << '\n';
}

// --------------------------------------------------
// Compilação dos shaders
// --------------------------------------------------

GLuint compileShader(
    GLenum type,
    const char* source
) {
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char infoLog[1024];

        glGetShaderInfoLog(
            shader, sizeof(infoLog),
            nullptr, infoLog
        );

        std::cerr << "Erro ao compilar shader:\n"
                  << infoLog << '\n';

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint criarShaderProgram() {
    const char* vertexSource = R"(
        #version 400 core

        layout(location = 0) in vec3 vertexPosition;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main() {
            gl_Position =
                projection * view * model
                * vec4(vertexPosition, 1.0);
        }
    )";

    const char* fragmentSource = R"(
        #version 400 core

        out vec4 fragColor;

        uniform vec3 groundColor;

        void main() {
            fragColor = vec4(groundColor, 1.0);
        }
    )";

    GLuint vertexShader = compileShader(
        GL_VERTEX_SHADER, vertexSource
    );

    GLuint fragmentShader = compileShader(
        GL_FRAGMENT_SHADER, fragmentSource
    );

    if (vertexShader == 0 || fragmentShader == 0) {
        if (vertexShader != 0) {
            glDeleteShader(vertexShader);
        }

        if (fragmentShader != 0) {
            glDeleteShader(fragmentShader);
        }

        return 0;
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        char infoLog[1024];

        glGetProgramInfoLog(
            program, sizeof(infoLog),
            nullptr, infoLog
        );

        std::cerr << "Erro ao linkar shaders:\n"
                  << infoLog << '\n';

        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// --------------------------------------------------
// Entrada do usuário
// --------------------------------------------------

void processInput(float deltaTime) {
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE)
        == GLFW_PRESS) {
        glfwSetWindowShouldClose(Window, true);
    }

    if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.processKeyboard(GLFW_KEY_W, deltaTime);
    }

    if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.processKeyboard(GLFW_KEY_S, deltaTime);
    }

    if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboard(GLFW_KEY_A, deltaTime);
    }

    if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboard(GLFW_KEY_D, deltaTime);
    }
}

// --------------------------------------------------
// Loop principal
// --------------------------------------------------

void inicializaRenderizacao(
    GLuint shaderProgram,
    Ground& ground
) {
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(Window)) {
        float currentFrame =
            static_cast<float>(glfwGetTime());

        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(deltaTime);

        glClearColor(0.45f, 0.65f, 0.85f, 1.0f);

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );

        glViewport(0, 0, screenWidth, screenHeight);

        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 view = camera.getViewMatrix();

        float aspectRatio =
            static_cast<float>(screenWidth) /
            static_cast<float>(screenHeight);

        glm::mat4 projection = glm::perspective(
            glm::radians(60.0f),
            aspectRatio,
            0.1f,
            300.0f
        );

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "model"),
            1, GL_FALSE, glm::value_ptr(model)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "view"),
            1, GL_FALSE, glm::value_ptr(view)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "projection"),
            1, GL_FALSE, glm::value_ptr(projection)
        );

        // Verde para o chão.
        glUniform3f(
            glGetUniformLocation(
                shaderProgram, "groundColor"
            ),
            0.25f, 0.55f, 0.25f
        );

        ground.draw();

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }
}

// --------------------------------------------------
// main
// --------------------------------------------------

int main() {
    inicializaOpenGL();

    GLuint shaderProgram = criarShaderProgram();

    if (shaderProgram == 0) {
        glfwDestroyWindow(Window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    {
        Ground ground;

        inicializaRenderizacao(shaderProgram, ground);
    }

    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(Window);
    glfwTerminate();

    return EXIT_SUCCESS;
}