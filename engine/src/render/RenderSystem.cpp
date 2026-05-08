//-----------------------------------
// Renderer.cpp
// Caleb Davis
// 
//-------------------------------------

#include "engine/render/RenderSystem.h"
#include "engine/core/ServiceLocator.h"
#include "engine/core/WindowSystem.h"
#include <glad/glad.h>
#include <cstdio>

SEResult RenderSystem::Init() {
    //Adds context depending on cmake
    auto* windowSystem = ServiceLocator::Get<WindowSystem>();
    ENGINE_ASSERT(windowSystem != nullptr, "WindowSystem must be registered before RenderSystem");

    m_context = GFXContext::Create();
    if (!m_context)
        return SEResult::fail("Failed to create GFX context");

    GLFWwindow* window = static_cast<GLFWwindow*>(windowSystem->GetNativeHandle());
    SEResult result = m_context->Init(window);
    if (!result)
        return result;

    printf("[Renderer] Initialized\n");
    return SEResult::ok();
}

void RenderSystem::Shutdown() {
    if (m_context)
        m_context->Shutdown();
    m_initialized = false;
}

void RenderSystem::BeginFrame() {
    // glClear moves here from Application
    //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::EndFrame() {
    m_context->Present();
}

void RenderSystem::Submit(CommandBuffer& cmd) {
    m_context->Submit(cmd);
}