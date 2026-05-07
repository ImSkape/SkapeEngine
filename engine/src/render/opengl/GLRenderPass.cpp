//-----------------------------------
// GLRenderPass.cpp
// Caleb Davis
// Tells where to render to for opengl
//-------------------------------------

#include "engine/render/opengl/GLRenderPass.h"
#include <glad/glad.h>
#include <cstdio>

GLRenderPass::GLRenderPass(const RenderPassSpec& spec) {
    m_spec = spec;

    // width/height of 0 means render to window
    if (spec.width == 0 || spec.height == 0) {
        m_isDefault = true;
        m_fbo = 0;
        return;  // nothing else needed � FBO 0 is provided by OpenGL
    }
    // offscreen � create our own FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // colour attachment
    glGenTextures(1, &m_colorTex);
    glBindTexture(GL_TEXTURE_2D, m_colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, spec.width, spec.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTex, 0);

    // depth + stencil attachment
    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
        spec.width, spec.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, m_rbo);

    // verify the FBO is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("[RenderPass] Framebuffer incomplete\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLRenderPass::~GLRenderPass() {
    if (!m_isDefault) {
        if (m_colorTex != 0) {
            glDeleteTextures(1, &m_colorTex);
            m_colorTex = 0;
        }
        if (m_rbo != 0) {
            glDeleteRenderbuffers(1, &m_rbo);
            m_rbo = 0;
        }
        if (m_fbo != 0) {
            glDeleteFramebuffers(1, &m_fbo);
            m_fbo = 0;
        }
    }
}

void GLRenderPass::Begin() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    if (!m_isDefault)
        glViewport(0, 0, m_spec.width, m_spec.height);

    // clear based on attachment specs
    if (!m_spec.colorAttachments.empty()) {
        const auto& col = m_spec.colorAttachments[0];
        if (col.loadOp == LoadOp::Clear) {
            glClearColor(col.clearColor.r, col.clearColor.g,
                col.clearColor.b, col.clearColor.a);
            glClear(GL_COLOR_BUFFER_BIT);
        }
    }

    if (m_spec.depthAttachment.loadOp == LoadOp::Clear) {
        glClearDepth(m_spec.depthAttachment.clearDepth);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}





void GLRenderPass::End() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}