#include <cstdlib>
#include <format>
#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game/Game.h"

constexpr GLint  kWindowWidth   = 1280;
constexpr GLint  kWindowHeight  = 960;
constexpr GLchar kWindowTitle[] = "Breakout FPS:";

Game* kBreakout = nullptr;

GLvoid FramebufferSizeCallback(GLFWwindow* Window, GLint Width, GLint Height);
GLvoid KeyCallback(GLFWwindow* Window, GLint Key, GLint ScanCode, GLint Action, GLint Mods);
GLvoid MessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, const GLchar* Message, const GLvoid* UserParam);
GLvoid Terminate(GLFWwindow* Window);

int main() {
    if (!glfwInit()) {
        std::cerr << "Error: Failed to initialize GLFW." << std::endl;
        std::system("pause");
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* Window = glfwCreateWindow(kWindowWidth, kWindowHeight, kWindowTitle, nullptr, nullptr);
    if (!Window) {
        std::cerr << "Error: Failed to create GLFW window." << std::endl;
        glfwTerminate();
        std::system("pause");
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(Window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Error: Failed to initialize Glad." << std::endl;
        glfwTerminate();
        std::system("pause");
        return EXIT_FAILURE;
    }

    // glfwSwapInterval(GLFW_FALSE);

    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
    glfwSetKeyCallback(Window, KeyCallback);
    glDebugMessageCallback(MessageCallback, nullptr);

    glViewport(0, 0, kWindowWidth, kWindowHeight);
    glEnable(GL_BLEND);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    kBreakout = new Game(kWindowWidth, kWindowHeight);

    GLdouble CurrentTime   = 0.0;
    GLdouble PreviousTime  = glfwGetTime();
    GLdouble LastFrameTime = 0.0;
    GLdouble DeltaTime     = 0.0;
    GLint    FrameCount    = 0;

    while (!glfwWindowShouldClose(Window)) {
        ++FrameCount;
        CurrentTime   = glfwGetTime();
        DeltaTime     = CurrentTime - LastFrameTime;
        LastFrameTime = CurrentTime;
        if (CurrentTime - PreviousTime >= 1.0) {
            glfwSetWindowTitle(Window, (std::string(kWindowTitle) + " " + std::to_string(FrameCount)).c_str());
            FrameCount   = 0;
            PreviousTime = CurrentTime;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        kBreakout->ProcessInput(static_cast<GLfloat>(DeltaTime));
        kBreakout->Update(static_cast<GLfloat>(DeltaTime));
        kBreakout->Render();

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    Terminate(Window);

    return EXIT_SUCCESS;
}

GLvoid FramebufferSizeCallback(GLFWwindow* Window, GLint Width, GLint Height) {
    glViewport(0, 0, Width, Height);
}

GLvoid KeyCallback(GLFWwindow* Window, GLint Key, GLint ScanCode, GLint Action, GLint Mods) {
    if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS) {
        glfwSetWindowShouldClose(Window, GLFW_TRUE);
    }

    if (Key >= 0 && Key <= 1024) {
        if (Action == GLFW_PRESS) {
            kBreakout->SetKey(Key, GL_TRUE);
        } else if (Action == GLFW_RELEASE) {
            kBreakout->SetKey(Key, GL_FALSE);
            kBreakout->SetProcessedKey(Key, GL_FALSE);
        }
    }
}

GLvoid MessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, const GLchar* Message, const GLvoid* UserParam) {
    auto SourceStr = [Source]() -> std::string {
        switch (Source) {
        case GL_DEBUG_SOURCE_API:
            return "API";
        case GL_DEBUG_SOURCE_APPLICATION:
            return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER:
            return "OTHER";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "SHADER";
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "THIRD_PARTY";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "WINDOW_SYSTEM";
        default:
            return "";
        }
    }();

    auto TypeStr = [Type]() -> std::string {
        switch (Type) {
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";
        case GL_DEBUG_TYPE_MARKER:
            return "MARKER";
        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";
        default:
            return "";
        }
    }();

    auto SeverityStr = [Severity]() -> std::string {
        switch (Severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            return "HIGH";
        case GL_DEBUG_SEVERITY_LOW:
            return "LOW";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "MEDIUM";
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "NOTIFICATION";
        default:
            return "";
        }
    }();

    std::cerr << std::format("Source: {}, Type: {}, Severity: {}\n{}: {}", SourceStr, TypeStr, SeverityStr, Id, Message)
              << std::endl;
}

GLvoid Terminate(GLFWwindow* Window) {
    delete kBreakout;
    kBreakout = nullptr;

    glfwDestroyWindow(Window);
    glfwTerminate();
}
