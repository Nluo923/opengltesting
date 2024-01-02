// Local Headers
#include "glitter.hpp"

// System Headers
#include <GLFW/glfw3.h>
#include <glad/glad.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

const char *ROOT = "../Glitter/";

GLuint create_shader_program_from_strings(
    const char *vertexShaderSource,
    const char *fragmentShaderSource) {
    assert(vertexShaderSource && fragmentShaderSource);
    uint vertexShader, fragmentShader;
    uint shaderProgram;

    {
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            fprintf(stderr, "Failed vertexshader.compilation: %s\n", infoLog);
        }
    }
    {
        int success;
        char infoLog[512];
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            fprintf(stderr, "Failed fragmentshader.compilation: %s", infoLog);
        }
    }
    {
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);

        glLinkProgram(shaderProgram);

        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            fprintf(stderr, "Failed program.linking: %s", infoLog);
        }
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint create_shader_program_from_files(
    const char *vertexShaderFile,
    const char *fragmentShaderFile) {
    assert(vertexShaderFile && fragmentShaderFile);

    const int MAX_SHADER_LENGTH = 1048576;
    char vertexShaderSource[MAX_SHADER_LENGTH], fragmentShaderSource[MAX_SHADER_LENGTH];

    { // read vertex shader file into a buffer
        std::string path = (std::string(ROOT) + vertexShaderFile);
        std::ifstream fs(path, std::ios::binary);
        fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        if (fs.fail()) {
            fprintf(stderr, "Failed to open vertexshader `%s%s`\n",
                    ROOT, vertexShaderFile);
        }

        fs.seekg(0, std::ios::end);
        size_t s = fs.tellg();
        fs.seekg(0);
        std::cout << "Reading " << s << " bytes\n";

        vertexShaderSource[0] = '\0';
        try {
            fs.read(&vertexShaderSource[0], s);
            vertexShaderSource[s] = '\0';

            std::cout << vertexShaderSource << "\n";
        } catch (const std::system_error &e) {
            fprintf(stderr, "Err: %s\n", e.code().message().c_str());
        }

        fs.close();
    }
    std::cout << " --- \n";
    { // read fragment shader file into a buffer
        std::string path = (std::string(ROOT) + fragmentShaderFile);
        std::ifstream fs(path, std::ios::binary);
        fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        if (fs.fail()) {
            fprintf(stderr, "Failed to open fragmentshader `%s%s`\n",
                    ROOT, fragmentShaderFile);
        }

        fs.seekg(0, std::ios::end);
        size_t s = fs.tellg();
        fs.seekg(0);
        std::cout << "Reading " << s << " bytes\n";

        fragmentShaderSource[0] = '\0';
        try {
            fs.read(&fragmentShaderSource[0], s);
            fragmentShaderSource[s] = '\0';

            std::cout << fragmentShaderSource << "\n";
        } catch (const std::system_error &e) {
            fprintf(stderr, "Err: %s\n", e.code().message().c_str());
        }

        fs.close();
    }

    return create_shader_program_from_strings(vertexShaderSource, fragmentShaderSource);
}

void reload_shader_program_from_files(
    GLuint *program,
    const char *vertexShaderFile,
    const char *fragmentShaderFile) {
    assert(program && vertexShaderFile && fragmentShaderFile);

    std::cout << "\n\n--- Reloading shaders... ---\n";
    GLuint reloaded_program = create_shader_program_from_files(
        vertexShaderFile, fragmentShaderFile);

    if (reloaded_program) {
        std::cout << "Reloaded shaders successfully.";
        glDeleteProgram(*program);
        *program = reloaded_program;
    }
}

int main(int argc, char *argv[]) {
    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    // Setup
    glViewport(0, 0, mWidth, mHeight);

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(mWindow, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    glfwSetFramebufferSizeCallback(mWindow, [](GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); });

    // Shader
    std::cout << argv[0] << "\n";
    uint shaderProgram = create_shader_program_from_files("Shaders/main.vs", "Shaders/main.fs");

    // Vertex
    float vertices[] = {
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f};

    uint indices[] = {
        0, 1, 3,
        1, 2, 3};

    uint VBO;
    glGenBuffers(1, &VBO);

    uint VAO;
    glGenVertexArrays(1, &VAO);

    uint EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex Attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Input Processing
    bool prevInputs[GLFW_KEY_LAST] = {false};
    auto processInput = [&]() {
        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(mWindow, true);
        }
        if (glfwGetKey(mWindow, GLFW_KEY_R)) {
            if (!prevInputs[GLFW_KEY_R])
                reload_shader_program_from_files(&shaderProgram, "Shaders/main.vs", "Shaders/main.fs");
            prevInputs[GLFW_KEY_R] = true;
        } else {
            prevInputs[GLFW_KEY_R] = false;
        }
    };

    // Rendering Loop
    while (!glfwWindowShouldClose(mWindow)) {
        processInput();

        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Flip Buffers and Draw
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
