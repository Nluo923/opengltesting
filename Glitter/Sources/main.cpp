// Local Headers
#include "glitter.hpp"
#include "shader.hpp"

// System Headers
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

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
    Shader main_shader = Shader("Shaders/main.vs", "Shaders/main.fs");

    // Vertex
    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // pos

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // color

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(2 * 3 * sizeof(float)));
    glEnableVertexAttribArray(1); // texture coords

    glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON);

    // Input Processing
    bool prevInputs[GLFW_KEY_LAST] = {false};
    auto processInput = [&]() {
        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(mWindow, true);
        }
        if (glfwGetKey(mWindow, GLFW_KEY_R)) {
            if (!prevInputs[GLFW_KEY_R])
                main_shader.reload_shader_program_from_files("Shaders/main.vs", "Shaders/main.fs");
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

        glUseProgram(main_shader.program);

        float per = sin(glfwGetTime()) / 2.0f + 0.5f;
        int u_time = glGetUniformLocation(main_shader.program, "time");
        glUniform1f(u_time, per);

        glBindVertexArray(VAO);

        // Flip Buffers and Draw
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
