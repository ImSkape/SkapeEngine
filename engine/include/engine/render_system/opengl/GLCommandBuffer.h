//-----------------------------------
// GLCommandBuffer.h
// Caleb Davis
// OpenGL implementation from graphics API
//-------------------------------------

#pragma once
#include "engine/render_system/CommandBuffer.h"

class GLCommandBuffer : public CommandBuffer {
public:
    ~GLCommandBuffer();

    void Begin()  override;
    void End()    override;
    void Reset()  override;

    void BeginRenderPass(RenderPass& pass) override;
    void EndRenderPass() override;
    void SetTopology(PrimitiveTopology topology) override;
    void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;
    void SetScissor(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;


    void SetShader(GPUShader& shader) override;
    void BindVertexBuffer(GPUBuffer& buffer, uint32_t slot = 0) override;
    void BindIndexBuffer(GPUBuffer& buffer) override;
    void BindTexture(GPUTexture& texture, uint32_t slot)  override;
    void Draw(uint32_t vertexCount, uint32_t firstVertex = 0) override;
    void DrawIndexed(uint32_t indexCount, uint32_t firstIndex = 0, int32_t  vertexOffset = 0) override;

private:
    void SetupVertexAttributes();

    uint32_t m_vao = 0;
    uint32_t m_glTopology = 0;
    RenderPass* m_renderPass = nullptr;
};