#ifndef SHADER_H
#define SHADER_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

const char *ROOT = "../Glitter/";

class Shader {
public:
    GLuint program;
    Shader(const char *vertexShaderFile,
           const char *fragmentShaderFile) {
        program = create_shader_program_from_files(vertexShaderFile, fragmentShaderFile);
    }

    void reload_shader_program_from_files(
        const char *vertexShaderFile,
        const char *fragmentShaderFile) {
        assert(vertexShaderFile && fragmentShaderFile);

        std::cout << "\n\n--- Reloading shaders... ---\n";
        GLuint reloaded_program = create_shader_program_from_files(
            vertexShaderFile, fragmentShaderFile);

        if (reloaded_program) {
            std::cout << "Reloaded shaders successfully.";
            glDeleteProgram(program);
            program = reloaded_program;
        }
    }

    void setUniform(const std::string &uniform, bool value) const {
        glUniform1i(glGetUniformLocation(program, uniform.c_str()), (int)value);
    }

    void setUniform(const std::string &uniform, int value) const {
        glUniform1i(glGetUniformLocation(program, uniform.c_str()), value);
    }

    void setUniform(const std::string &uniform, float value) const {
        glUniform1f(glGetUniformLocation(program, uniform.c_str()), value);
    }

private:
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
            std::cout << "Reading " << s << " chars\n";

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
            std::cout << "Reading " << s << " chars\n";

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
};

#endif