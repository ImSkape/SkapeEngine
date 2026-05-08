//-----------------------------------
// WindowGlfw.cpp
// Caleb Davis
// glfw window implementation
//-------------------------------------

#include "engine/core/SECore.h"
#include "engine/platform/glfw/WindowGlfw.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "engine/core/SEResult.h"


SEResult WindowGlfw::Init(int width, int height, const std::string& title)
{
    //Initialize glfw
    if (!glfwInit()) {
		return SEResult::fail("Failed to initialize GLFW");
    }



    //Set up version and debug mode
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // required on Mac

    //Create the window
    m_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    //Window handle
    if (!m_window) {
        glfwTerminate();
        return SEResult::fail("GLFW window failed to be created");
    }

    //Make the window's context current 
    glfwMakeContextCurrent(m_window);

    //Load in GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return SEResult::fail("GLAD initialization failed");
    }

    //Load the OpenGL debugger and conntect it to console
    SetupOpenGLDebugCallback();

    
    return SEResult::ok();
}

void WindowGlfw::PollEvents()
{
    glfwPollEvents();
}

bool WindowGlfw::ShouldClose()
{
	return glfwWindowShouldClose(m_window);
}

void WindowGlfw::SwapBuffers()
{
    glfwSwapBuffers(m_window);
}

void WindowGlfw::Shutdown()
{
    glfwTerminate();
}


GLFWwindow* WindowGlfw::GetGLFWWindow()
{
    return m_window;
}

int WindowGlfw::GetWidth() {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return width;
}

int WindowGlfw::GetHeight() {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return height;
}

bool WindowGlfw::IsFocused() {
    return glfwGetWindowAttrib(m_window, GLFW_FOCUSED);
}

void* WindowGlfw::GetNativeHandle() {
    return static_cast<void*>(m_window);
}



void WindowGlfw::SetupOpenGLDebugCallback()
{
    //Enable debug output messages
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    //Setup callback
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id,
        GLenum severity, GLsizei length,
        const GLchar* message, const void* userParam) {
            UNREFERENCED_PARAMETER(source);
            UNREFERENCED_PARAMETER(type);
            UNREFERENCED_PARAMETER(id);
            UNREFERENCED_PARAMETER(length);
            UNREFERENCED_PARAMETER(userParam);
            if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
                printf("[GL] %s\n", message);
            else {
                printf("[GLMsg] %s\n", message);
            }
        }, nullptr);

    //Insert a validation message
    glDebugMessageInsert(
        GL_DEBUG_SOURCE_APPLICATION,
        GL_DEBUG_TYPE_MARKER,
        0,
        GL_DEBUG_SEVERITY_NOTIFICATION,
        -1,
        "OpenGL debug system initialized"
    );
}
