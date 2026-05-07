//-----------------------------------
// Renderer.cpp
// Caleb Davis
// 
//-------------------------------------

#include "engine/render_system/RenderSystem.h"
#include <glad/glad.h>
#include <cstdio>

SEResult Renderer::Init(GLFWwindow* window) {
    //Adds context depending on cmake
    m_context = GFXContext::Create();
    if (!m_context)
        return SEResult::fail("Failed to create GFX context");

    SEResult result = m_context->Init(window);
    if (!result)
        return result;

    printf("[Renderer] Initialized\n");
    return SEResult::ok();
}

void Renderer::Shutdown() {
    if (m_context)
        m_context->Shutdown();
}

void Renderer::BeginFrame() {
    // glClear moves here from Application
    //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndFrame() {
    m_context->Present();
}

void Renderer::Submit(CommandBuffer& cmd) {
    m_context->Submit(cmd);
}