//-----------------------------------
// GPUBuffer.h
// Caleb Davis
// Holds data on gpu
//-------------------------------------

#pragma once
#include <cstdint>

enum class BufferType {
    Vertex,
    Index,
    Uniform
};

enum class BufferUsage {
    Static,   // uploaded once, never changes
    Dynamic,  // updated frequently
    Stream    // updated every frame
};

class GPUBuffer {
public:
    virtual ~GPUBuffer() = default;

    virtual void Upload(const void* data, uint32_t size) = 0;
    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    uint32_t  GetSize()   const { return m_size; }
    BufferType GetType()  const { return m_type; }

protected:
    uint32_t   m_size = 0;
    BufferType m_type = BufferType::Vertex;
    BufferUsage m_usage = BufferUsage::Static;
};