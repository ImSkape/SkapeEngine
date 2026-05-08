//-----------------------------------
// GLContext.cpp
// Caleb Davis
//
//-------------------------------------

#include "engine/core/SECore.h"
#include "engine/render/opengl/GLContext.h"
#include "engine/render/opengl/GLBuffer.h"
#include "engine/render/opengl/GLShader.h"
#include "engine/render/opengl/GLRenderPass.h"
#include "engine/render/opengl/GLCommandBuffer.h"
#include "engine/core/ServiceLocator.h"
#include "engine/core/WindowSystem.h"
#include <glad/glad.h>
#include <cstdio>
#include <memory>

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
    auto* windowSystem = ServiceLocator::Get<WindowSystem>();
    ENGINE_ASSERT(windowSystem != nullptr, "WindowSystem not registered");
    windowSystem->Present();
}

void GLContext::Submit(CommandBuffer& cmd) {
    // no-op for OpenGL � commands execute immediately when called
    // this is where real work happens in Vulkan
    (void)cmd;
}

// stubs for now � implement each when you get to that object
std::unique_ptr<GPUBuffer> GLContext::CreateBuffer(
    BufferType type, BufferUsage usage, uint32_t size) {
    return std::make_unique<GLBuffer>(type, usage, size);
}

std::unique_ptr<GPUShader> GLContext::CreateShader(
    const std::string& vert, const std::string& frag) {
    return std::make_unique<GLShader>(vert, frag);
}

std::unique_ptr<RenderPass> GLContext::CreateRenderPass(
    const RenderPassSpec& spec) {
    return std::make_unique<GLRenderPass>(spec);
}

std::unique_ptr<CommandBuffer> GLContext::CreateCommandBuffer() {
    return std::make_unique<GLCommandBuffer>();
}
std::unique_ptr<GPUTexture> GLContext::CreateTexture(const TextureSpec& spec) {
    UNREFERENCED_PARAMETER(spec);
    // TODO: implement GLTexture
    return nullptr;
}