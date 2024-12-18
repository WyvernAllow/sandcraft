#include <spdlog/spdlog.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "camera.hpp"

static void glfw_error_callback(int error_code, const char* description) {
    spdlog::error("GLFW: {}", description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width,
                                      int height) {
    glViewport(0, 0, width, height);
}

static std::string read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::ios_base::failure("Failed to open file: " + filename);
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer(size, '\0');
    if (!file.read(buffer.data(), size)) {
        throw std::ios_base::failure("Failed to read file: " + filename);
    }

    return buffer;
}

static GLuint compile_shader(const std::string& filename, GLenum type) {
    const std::string source = read_file(filename);
    const char* source_cstr = source.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source_cstr, nullptr);
    glCompileShader(shader);

    GLint did_compile;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &did_compile);
    if (!did_compile) {
        char info_log[256];
        glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);

        glDeleteShader(shader);

        throw std::runtime_error(
            fmt::format("Failed to compile {}: {}", filename, info_log));
    }

    return shader;
}

sc::camera camera;

static void glfw_scroll_callback(GLFWwindow* window, double x, double y) {
    if (y < 0.0f) {
        camera.zoom *= 0.9;
    }

    if (y > 0.0f) {
        camera.zoom /= 0.9;
    }
}

struct vertex {
    float position[2];
    float texcoord[2];
};

static void run() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        throw std::runtime_error("glfwInit failed");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, false);

    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window =
        glfwCreateWindow(800, 450, "Sandcraft", nullptr, nullptr);
    if (!window) {
        throw std::runtime_error("glfwCreateWindow failed");
    }

    GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);

    glfwSetWindowMonitor(window, primary_monitor, 0, 0, mode->width,
                         mode->height, mode->refreshRate);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, glfw_scroll_callback);

    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress)) {
        throw std::runtime_error("gladLoadGL failed");
    }

    glViewport(0, 0, mode->width, mode->height);

    glEnable(GL_MULTISAMPLE);

    std::vector<vertex> vertices = {
        {{0.0f, 0.0f}, {0.0f, 1.0f}},
        {{128.0f, 0.0f}, {0.0f, 0.0f}},
        {{128.0f, 128.0f}, {1.0f, 0.0f}},
        {{0.0f, 128.0f}, {1.0f, 1.0f}},
    };

    std::vector<GLuint> indices = {0, 1, 2, 0, 2, 3};

    GLuint vert = compile_shader("res/shaders/chunk.vert", GL_VERTEX_SHADER);
    GLuint frag = compile_shader("res/shaders/chunk.frag", GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    GLint did_link;
    glGetProgramiv(program, GL_LINK_STATUS, &did_link);
    if (!did_link) {
        char info_log[256];
        glGetProgramInfoLog(program, sizeof(info_log), nullptr, info_log);

        throw std::runtime_error(
            fmt::format("Failed to link shaders: {}", info_log));
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(),
                 vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
        reinterpret_cast<const void*>(offsetof(vertex, position)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
        reinterpret_cast<const void*>(offsetof(vertex, texcoord)));

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(),
                 indices.data(), GL_STATIC_DRAW);

    camera.x = 0.0f;
    camera.y = 0.0f;
    camera.center_x = 800.0f / 2.0f;
    camera.center_y = 450.0f / 2.0f;
    camera.zoom = 1.0f;

    float current_time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        float new_time = glfwGetTime();
        float delta_time = new_time - current_time;
        current_time = new_time;

        if (glfwGetKey(window, GLFW_KEY_W)) {
            camera.y -= delta_time * 16.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_S)) {
            camera.y += delta_time * 16.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_D)) {
            camera.x += delta_time * 16.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_A)) {
            camera.x -= delta_time * 16.0f;
        }

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);

        const float* mvp = camera.get_view_proj(800.0f, 450.0f).data();

        glUniformMatrix4fv(glGetUniformLocation(program, "u_mvp"), 1, GL_FALSE,
                           mvp);

        glBindVertexArray(vao);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int argc, char** argv) {
    try {
        run();
    } catch (std::exception& e) {
        spdlog::critical("Fatal exception: {}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}