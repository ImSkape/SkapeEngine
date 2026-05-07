//-----------------------------------
// Stub.cpp
// Caleb Davis
// stub
//-------------------------------------

#pragma once
#include "engine/render_system/GFXContext.h"

struct GLFWwindow;

class GLContext : public GFXContext {
public:
    SEResult Init(GLFWwindow* window) override;
    void     Shutdown()                  override;
    void     Present()                   override;
    void     Submit(CommandBuffer& cmd) override;

    std::unique_ptr<GPUBuffer>     CreateBuffer(BufferType type, BufferUsage usage, uint32_t size)              override;
    std::unique_ptr<GPUShader>     CreateShader(const std::string& vertSrc, const std::string& fragSrc)        override;
    std::unique_ptr<GPUTexture>    CreateTexture(const TextureSpec& spec)                                        override;
    std::unique_ptr<RenderPass>    CreateRenderPass(const RenderPassSpec& spec)                                     override;
    std::unique_ptr<CommandBuffer> CreateCommandBuffer()                                                               override;

private:
    GLFWwindow* m_window = nullptr;
};