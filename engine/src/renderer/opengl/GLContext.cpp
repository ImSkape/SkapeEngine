//-----------------------------------
// GLContext.cpp
// Caleb Davis
//
//-------------------------------------

#include "engine/renderer/opengl/GLContext.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>

SEResult GLContext::Init(GLFWwindow* window) {
    m_window = window;
    m_backend = GFXBackend::OpenGL;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    printf("[GLContext] OpenGL %s\n", glGetString(GL_VERSION));
    return SEResult::ok();
}

void GLContext::Shutdown() {
    m_window = nullptr;
}

void GLContext::Present() {
    glfwSwapBuffers(m_window);
}

void GLContext::Submit(CommandBuffer& cmd) {
    // no-op for OpenGL — commands execute immediately when called
    // this is where real work happens in Vulkan
    (void)cmd;
}

// stubs for now — implement each when you get to that object
std::unique_ptr<GPUBuffer>     GLContext::CreateBuffer(BufferType, BufferUsage, uint32_t) { return nullptr; }
std::unique_ptr<GPUShader>     GLContext::CreateShader(const std::string&, const std::string&) { return nullptr; }
std::unique_ptr<GPUTexture>    GLContext::CreateTexture(const TextureSpec&) { return nullptr; }
std::unique_ptr<RenderPass>    GLContext::CreateRenderPass(const RenderPassSpec&) { return nullptr; }
std::unique_ptr<CommandBuffer> GLContext::CreateCommandBuffer() { return nullptr; }