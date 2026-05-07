//-----------------------------------
// GLShader.h
// Caleb Davis
// opengl shader
//-------------------------------------

#pragma once
#include "engine/render/GPUShader.h"

class GLShader : public GPUShader {
public:
    GLShader(const std::string& vertSrc, const std::string& fragSrc);
    ~GLShader();

    void Bind()   override;
    void Unbind() override;

    void SetInt(const std::string& name, int value)            override;
    void SetFloat(const std::string& name, float value)          override;
    void SetFloat2(const std::string& name, const glm::vec2& v)   override;
    void SetFloat3(const std::string& name, const glm::vec3& v)   override;
    void SetFloat4(const std::string& name, const glm::vec4& v)   override;
    void SetMat3(const std::string& name, const glm::mat3& m)   override;
    void SetMat4(const std::string& name, const glm::mat4& m)   override;

private:
    uint32_t m_handle = 0;

    uint32_t CompileShader(uint32_t type, const std::string& src);
    int      GetUniformLocation(const std::string& name) const;
};