//-----------------------------------
// RenderSystem.h
// Caleb Davis
// stub
//-------------------------------------

#pragma once
#include <memory>
#include <string>
#include "engine/core/System.h"
#include "engine/render/GFXContext.h"
#include "engine/render/CommandBuffer.h"
#include "engine/core/SEResult.h"

struct GLFWwindow;

class RenderSystem : public System {
public:
    SEResult Init() override;
    void     Shutdown() override;
    void     BeginFrame();
    void     EndFrame();

    GFXContext& GetContext() { return *m_context; }
    void Submit(CommandBuffer& cmd);

    const std::string& GetName() const override {
        static std::string name = "RenderSystem";
        return name;
    }

private:
    std::unique_ptr<GFXContext>    m_context;
};