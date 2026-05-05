//-----------------------------------
// CommandBuffer.h
// Caleb Davis
// Holds most of the API calls
//-------------------------------------

#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "engine/renderer/GPUBuffer.h"
#include "engine/renderer/GPUShader.h"
#include "engine/renderer/GPUTexture.h"
#include "engine/renderer/RenderPass.h"

// how vertices are assembled into primitives
enum class PrimitiveTopology {
    Triangles,
    Lines,
    Points,
    TriangleStrip
};

class CommandBuffer {
public:
    virtual ~CommandBuffer() = default;

    // -- PASS -------------------------------------------
    virtual void BeginRenderPass(RenderPass& pass) = 0;
    virtual void EndRenderPass() = 0;

    // -- STATE ------------------------------------------
    virtual void SetShader(GPUShader& shader) = 0;
    virtual void SetTopology(PrimitiveTopology t) = 0;
    virtual void SetViewport(uint32_t x, uint32_t y,
        uint32_t w, uint32_t h) = 0;
    virtual void SetScissor(uint32_t x, uint32_t y,
        uint32_t w, uint32_t h) = 0;

    // -- RESOURCES -------------------------------------
    virtual void BindVertexBuffer(GPUBuffer& buffer,
        uint32_t slot = 0) = 0;
    virtual void BindIndexBuffer(GPUBuffer& buffer) = 0;
    virtual void BindTexture(GPUTexture& texture,
        uint32_t slot = 0) = 0;

    // -- DRAW ---------------------------------------
    virtual void Draw(uint32_t vertexCount,
        uint32_t firstVertex = 0) = 0;
    virtual void DrawIndexed(uint32_t indexCount,
        uint32_t firstIndex = 0,
        int32_t  vertexOffset = 0) = 0;

    // -- LIFECYCLE -------------------------------------------
    virtual void Begin() = 0;  // start recording
    virtual void End() = 0;  // stop recording
    virtual void Reset() = 0;  // clear all recorded commands
};