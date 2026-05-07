//-----------------------------------
// GLBuffer.cpp
// Caleb Davis
// Holds opengl data
//-------------------------------------

#include "engine/render/opengl/GLBuffer.h"
#include <glad/glad.h>

static uint32_t BufferTypeToGL(BufferType type);
static uint32_t BufferUsageToGL(BufferUsage usage);



GLBuffer::GLBuffer(BufferType type, BufferUsage usage, uint32_t size) {
    m_type = type;
    m_usage = usage;
    m_size = size;
    m_glTarget = BufferTypeToGL(type);
    m_glUsage = BufferUsageToGL(usage);

    glGenBuffers(1, &m_handle);
    glBindBuffer(m_glTarget, m_handle);
    glBufferData(m_glTarget, size, nullptr, m_glUsage);
    glBindBuffer(m_glTarget, 0);
}

GLBuffer::~GLBuffer() {
    if (m_handle != 0) {
        glDeleteBuffers(1, &m_handle);
        m_handle = 0;
    }
}

void GLBuffer::Upload(const void* data, uint32_t size) {
    glBindBuffer(m_glTarget, m_handle);
    glBufferSubData(m_glTarget, 0, size, data);
    glBindBuffer(m_glTarget, 0);
}

void GLBuffer::Bind() {
    glBindBuffer(m_glTarget, m_handle);
}

void GLBuffer::Unbind() {
    glBindBuffer(m_glTarget, 0);
}







static uint32_t BufferTypeToGL(BufferType type) {
    switch (type) {
    case BufferType::Vertex:  return GL_ARRAY_BUFFER;
    case BufferType::Index:   return GL_ELEMENT_ARRAY_BUFFER;
    case BufferType::Uniform: return GL_UNIFORM_BUFFER;
    default:                  return GL_ARRAY_BUFFER;
    }
}

static uint32_t BufferUsageToGL(BufferUsage usage) {
    switch (usage) {
    case BufferUsage::Static:  return GL_STATIC_DRAW;
    case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
    case BufferUsage::Stream:  return GL_STREAM_DRAW;
    default:                   return GL_STATIC_DRAW;
    }
}