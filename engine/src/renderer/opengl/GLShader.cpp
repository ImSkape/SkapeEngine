//-----------------------------------
// GLShader.cpp
// Caleb Davis
// opengl shader
//-------------------------------------

#include "engine/renderer/opengl/GLShader.h"
#include <glad/glad.h>
#include <cstdio>


GLShader::GLShader(const std::string& vertSrc, const std::string& fragSrc) {
    uint32_t vert = CompileShader(GL_VERTEX_SHADER, vertSrc);
    uint32_t frag = CompileShader(GL_FRAGMENT_SHADER, fragSrc);

    m_handle = glCreateProgram();
    glAttachShader(m_handle, vert);
    glAttachShader(m_handle, frag);
    glLinkProgram(m_handle);

    int success;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(m_handle, 512, nullptr, log);
        printf("[Shader] Link error: %s\n", log);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

GLShader::~GLShader() {
    if (m_handle != 0) {
        glDeleteProgram(m_handle);
        m_handle = 0;
    }
}



void GLShader::Bind() { 
    glUseProgram(m_handle);
}


void GLShader::Unbind() { 
    glUseProgram(0);
}


void GLShader::SetInt(const std::string& name, int value) {
    glUniform1i(GetUniformLocation(name), value);
}

void GLShader::SetFloat(const std::string& name, float value) {
    glUniform1f(GetUniformLocation(name), value);
}

void GLShader::SetFloat2(const std::string& name, const glm::vec2& v) {
    glUniform2fv(GetUniformLocation(name), 1, &v[0]);
}

void GLShader::SetFloat3(const std::string& name, const glm::vec3& v) {
    glUniform3fv(GetUniformLocation(name), 1, &v[0]);
}

void GLShader::SetFloat4(const std::string& name, const glm::vec4& v) {
    glUniform4fv(GetUniformLocation(name), 1, &v[0]);
}

void GLShader::SetMat3(const std::string& name, const glm::mat3& m) {
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &m[0][0]);
}

void GLShader::SetMat4(const std::string& name, const glm::mat4& m) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &m[0][0]);
}




int GLShader::GetUniformLocation(const std::string& name) const {
    int location = glGetUniformLocation(m_handle, name.c_str());
    if (location == -1)
        printf("[Shader] Uniform '%s' not found\n", name.c_str());
    return location;
}


 uint32_t GLShader::CompileShader(uint32_t type, const std::string& src) {
    uint32_t shader = glCreateShader(type);
    const char* cstr = src.c_str();
    glShaderSource(shader, 1, &cstr, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        printf("[Shader] Compile error: %s\n", log);
    }

    return shader;
}