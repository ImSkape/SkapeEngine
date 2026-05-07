//-----------------------------------
// GLBuffer.h
// Caleb Davis
// Holds opengl data
//-------------------------------------

#pragma once
#include "engine/render/GPUBuffer.h"

class GLBuffer : public GPUBuffer {
public:
    GLBuffer(BufferType type, BufferUsage usage, uint32_t size);
    ~GLBuffer();

    void Upload(const void* data, uint32_t size) override;
    void Bind()   override;
    void Unbind() override;

    uint32_t GetHandle() const { return m_handle; }

private:
    uint32_t m_handle = 0;
    uint32_t m_glTarget = 0;
    uint32_t m_glUsage = 0;
};