//-----------------------------------
// GLRenderPass.h
// Caleb Davis
// Tells where to render to for opengl
//-------------------------------------

#pragma once
#include "engine/render_system/RenderPass.h"

class GLRenderPass : public RenderPass {
public:
    GLRenderPass(const RenderPassSpec& spec);
    ~GLRenderPass();

    void Begin() override;
    void End()   override;

private:
    uint32_t m_fbo = 0;
    uint32_t m_colorTex = 0;
    uint32_t m_rbo = 0;
    bool     m_isDefault = false;
};