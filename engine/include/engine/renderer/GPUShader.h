//-----------------------------------
// GPUShader.h
// Caleb Davis
// API for Shader
//-------------------------------------

#pragma once
#include <string>
#include <glm/glm.hpp>

class GPUShader {
public:
    virtual ~GPUShader() = default;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    // uniform setters — engine never calls GL directly
    virtual void SetInt(const std::string& name, int value) = 0;
    virtual void SetFloat(const std::string& name, float value) = 0;
    virtual void SetFloat2(const std::string& name, const glm::vec2& v) = 0;
    virtual void SetFloat3(const std::string& name, const glm::vec3& v) = 0;
    virtual void SetFloat4(const std::string& name, const glm::vec4& v) = 0;
    virtual void SetMat3(const std::string& name, const glm::mat3& m) = 0;
    virtual void SetMat4(const std::string& name, const glm::mat4& m) = 0;
};