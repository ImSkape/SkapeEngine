//-----------------------------------
// Renderer.h
// Caleb Davis
// stub
//-------------------------------------

#pragma once
#include <memory>
#include "engine/render/GFXContext.h"
#include "engine/core/SEResult.h"

struct GLFWwindow;

class Renderer {
public:
    SEResult Init(GLFWwindow* window);
    void     Shutdown();
    void     BeginFrame();  // glClear moves here from Application
    void     EndFrame();    // calls Present()

    // access to context for creating GPU objects
    GFXContext& GetContext() { return *m_context; }

    // convenience � submit a command buffer
    void Submit(CommandBuffer& cmd);

private:
    std::unique_ptr<GFXContext>    m_context;
};