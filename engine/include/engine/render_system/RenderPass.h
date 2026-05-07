//-----------------------------------
// RenderPass.h
// Caleb Davis
// 
//-------------------------------------

#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "engine/render_system/GPUTexture.h"

enum class LoadOp {
    Clear,    // clear to clear value on begin
    Load,     // preserve existing contents
    DontCare  // don't care � fastest
};

enum class StoreOp {
    Store,    // write results out
    DontCare  // discard � fastest for depth/stencil
};

struct AttachmentSpec {
    TextureFormat format = TextureFormat::RGBA8;
    LoadOp        loadOp = LoadOp::Clear;
    StoreOp       storeOp = StoreOp::Store;
    glm::vec4     clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
    float         clearDepth = 1.0f;
};

struct RenderPassSpec {
    std::vector<AttachmentSpec> colorAttachments;
    AttachmentSpec              depthAttachment;
    uint32_t                    width = 0;
    uint32_t                    height = 0;
};

class RenderPass {
public:
    virtual ~RenderPass() = default;

    virtual void Begin() = 0;
    virtual void End() = 0;

    const RenderPassSpec& GetSpec() const { return m_spec; }

protected:
    RenderPassSpec m_spec;
};