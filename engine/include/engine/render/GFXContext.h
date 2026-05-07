//-----------------------------------
// GFXContext.h
// Caleb Davis
// Sets and handles the GFX context, backend vs. frontend
//-------------------------------------

#pragma once
#include <memory>
#include <string>
#include "engine/render/GPUBuffer.h"
#include "engine/render/GPUShader.h"
#include "engine/render/GPUTexture.h"
#include "engine/render/RenderPass.h"
#include "engine/render/CommandBuffer.h"
#include "engine/core/SEResult.h"

struct GLFWwindow;

// which backend to use � set via CMake -DGFX_BACKEND=OpenGL
enum class GFXBackend {
    OpenGL,
    Vulkan,
    Metal,
    D3D12
};

class GFXContext {
public:
    virtual ~GFXContext() = default;

    virtual SEResult Init(GLFWwindow* window) = 0;
    virtual void     Shutdown() = 0;
    virtual void     Present() = 0;  // swap buffers
    virtual void     Submit(CommandBuffer& cmd) = 0;  // execute recorded commands

    // factory methods � create backend-specific objects
    // caller gets owning pointer, never touches backend internals
    virtual std::unique_ptr<GPUBuffer>      CreateBuffer(BufferType type, BufferUsage usage, uint32_t size) = 0;
    virtual std::unique_ptr<GPUShader>      CreateShader(const std::string& vertSrc, const std::string& fragSrc) = 0;
    virtual std::unique_ptr<GPUTexture>     CreateTexture(const TextureSpec& spec) = 0;
    virtual std::unique_ptr<RenderPass>     CreateRenderPass(const RenderPassSpec& spec) = 0;
    virtual std::unique_ptr<CommandBuffer>  CreateCommandBuffer() = 0;

    GFXBackend GetBackend() const { return m_backend; }

    // static factory � creates the right context based on CMake define
    static std::unique_ptr<GFXContext> Create();

protected:
    GFXBackend m_backend = GFXBackend::OpenGL;
};