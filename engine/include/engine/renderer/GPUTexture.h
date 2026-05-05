//-----------------------------------
// Stub.h
// Caleb Davis
// API for textures
//-------------------------------------

#pragma once
#include <string>
#include <cstdint>

enum class TextureFormat {
    RGBA8,
    RGB8,
    R8,
    Depth24Stencil8
};

enum class TextureFilter {
    Nearest,
    Linear
};

enum class TextureWrap {
    Repeat,
    ClampToEdge,
    MirroredRepeat
};

struct TextureSpec {
    uint32_t      width = 1;
    uint32_t      height = 1;
    TextureFormat format = TextureFormat::RGBA8;
    TextureFilter filter = TextureFilter::Linear;
    TextureWrap   wrap = TextureWrap::Repeat;
    bool          genMips = true;
};

class GPUTexture {
public:
    virtual ~GPUTexture() = default;

    virtual void Bind(uint32_t slot = 0) = 0;
    virtual void Unbind() = 0;
    virtual void Upload(const void* data, uint32_t size) = 0;

    uint32_t GetWidth()  const { return m_spec.width; }
    uint32_t GetHeight() const { return m_spec.height; }
    const TextureSpec& GetSpec() const { return m_spec; }

protected:
    TextureSpec m_spec;
};