//-----------------------------------
// GLCommandBuffer.cpp
// Caleb Davis
// OpenGL implementation from graphics API
//-------------------------------------

#include "engine/renderer/opengl/GLCommandBuffer.h"
#include "engine/renderer/opengl/GLBuffer.h"
#include "engine/renderer/opengl/GLTexture.h"
#include "engine/renderer/opengl/GLRenderPass.h"
#include <glad/glad.h>

static uint32_t TopologyToGL(PrimitiveTopology t);

GLCommandBuffer::~GLCommandBuffer() {
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}

void GLCommandBuffer::Begin() {
    if (m_vao == 0)
        glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
}

void GLCommandBuffer::End() {
    glBindVertexArray(0);
}

void GLCommandBuffer::Reset() {
    // nothing to clear yet — more state gets added here later
}


void GLCommandBuffer::BeginRenderPass(RenderPass& pass) {
    m_renderPass = &pass;
    pass.Begin();
}

void GLCommandBuffer::EndRenderPass() {
    glDisable(GL_SCISSOR_TEST);  // reset scissor when pass ends
    if (m_renderPass)
        m_renderPass->End();
    m_renderPass = nullptr;
}




void GLCommandBuffer::SetTopology(PrimitiveTopology topology) {
    m_glTopology = TopologyToGL(topology);
}
void GLCommandBuffer::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    glViewport(x, y, w, h);
}
void GLCommandBuffer::SetScissor(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, w, h);
}

void GLCommandBuffer::SetShader(GPUShader& shader) {
    shader.Bind();
}

void GLCommandBuffer::BindVertexBuffer(GPUBuffer& buffer, uint32_t slot) {
    auto& glBuffer = static_cast<GLBuffer&>(buffer);
    glBindBuffer(GL_ARRAY_BUFFER, glBuffer.GetHandle());
    SetupVertexAttributes();
}

void GLCommandBuffer::BindIndexBuffer(GPUBuffer& buffer) {
    auto& glBuffer = static_cast<GLBuffer&>(buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glBuffer.GetHandle());
}

void GLCommandBuffer::BindTexture(GPUTexture& texture, uint32_t slot) {
    texture.Bind(slot);
}

void GLCommandBuffer::Draw(uint32_t vertexCount, uint32_t firstVertex) {
    glDrawArrays(m_glTopology, firstVertex, vertexCount);
}

void GLCommandBuffer::DrawIndexed(uint32_t indexCount,
    uint32_t firstIndex,
    int32_t  vertexOffset) {
    glDrawElements(m_glTopology, indexCount, GL_UNSIGNED_INT,
        (void*)(firstIndex * sizeof(uint32_t)));
}

void GLCommandBuffer::SetupVertexAttributes() {
    uint32_t stride = (3 + 3 + 2) * sizeof(float);

    // location 0 — position — 3 floats at offset 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
        (void*)0);

    // location 1 — normal — 3 floats at offset 12 bytes
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
        (void*)(3 * sizeof(float)));

    // location 2 — uv — 2 floats at offset 24 bytes
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
        (void*)(6 * sizeof(float)));
}


static uint32_t TopologyToGL(PrimitiveTopology t) {
    switch (t) {
    case PrimitiveTopology::Triangles:     return GL_TRIANGLES;
    case PrimitiveTopology::Lines:         return GL_LINES;
    case PrimitiveTopology::Points:        return GL_POINTS;
    case PrimitiveTopology::TriangleStrip: return GL_TRIANGLE_STRIP;
    default:                               return GL_TRIANGLES;
    }
}