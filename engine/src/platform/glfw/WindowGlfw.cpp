//-----------------------------------
// WindowGlfw.cpp
// Caleb Davis
// glfw window implementation
//-------------------------------------

#include "engine//platform/glfw/WindowGlfw.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "engine/core/SEResult.h"

SEResult WindowGlfw::Init(int width, int height, const std::string& title)
{
    if (!glfwInit()) {
		return SEResult::fail("Failed to initialize GLFW");
    }

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    if (!window) {
        glfwTerminate();
        return SEResult::fail("GLFW window failed to be created");
    }

    // Make the window's context current 
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return SEResult::fail("GLAD initialization failed");
    }


    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id,
        GLenum severity, GLsizei length,
        const GLchar* message, const void* userParam) {
            if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
                printf("[GL] %s\n", message);
        }, nullptr);

    return SEResult::ok();
}

void WindowGlfw::PollEvents()
{
    glfwPollEvents();
}

bool WindowGlfw::ShouldClose()
{
	return glfwWindowShouldClose(window);
}

void WindowGlfw::SwapBuffers()
{


    glfwSwapBuffers(window);
}

void WindowGlfw::Shutdown()
{
    glfwTerminate();
}
