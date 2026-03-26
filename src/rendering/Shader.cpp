#include <pch.h>
#include "Shader.h"

#include "Renderer.h"
#include <util/Util.h>

#include <GL/glew.h>

#include <malloc.h>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <string>

namespace pxr
{
    uint32_t CompileShader(uint32_t type, const std::string& source)
    {
        uint32_t id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            char* message = (char*)alloca(length * sizeof(char));
            glGetShaderInfoLog(id, length, &length, message);
            std::cout << "Failed to compile " <<
                (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<
                " shader" << std::endl;
            std::cout << message << std::endl;
            glDeleteShader(id);
            return 0;
        }

        return id;
    }

    uint32_t CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
    {
        glUseProgram(0);
        uint32_t program = glCreateProgram();
        uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
        uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        glValidateProgram(program);

        glDeleteShader(vs);
        glDeleteShader(fs);

        return program;
    }

    Shader::Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
    {
        std::string vertexSrc = StringFromFile(RelativePath(vertexPath));
        std::string fragmentSrc = StringFromFile(RelativePath(fragmentPath));

        m_RendererID = CreateShader(vertexSrc, fragmentSrc);
    }

    Shader::~Shader()
    {

    }

    void Shader::Use()
    {
        glUseProgram(m_RendererID);
    }

    void Shader::EndUse()
    {
        glUseProgram(0);
    }

    void Shader::SetUniformInt(const char* name, int value)
    {
        int32_t location = glGetUniformLocation(m_RendererID, name);
        ASSERT(location != -1);
        glUniform1i(location, value);
    }
    void Shader::SetUniformInt2(const char* name, int val1, int val2)
    {
        int32_t location = glGetUniformLocation(m_RendererID, name);
        ASSERT(location != -1);
        glUniform2i(location, val1, val2);
    }
    void Shader::SetUniformInt3(const char* name, int val1, int val2, int val3)
    {
        int32_t location = glGetUniformLocation(m_RendererID, name);
        ASSERT(location != -1);
        glUniform3i(location, val1, val2, val3);
    }
    void Shader::SetUniformInt4(const char* name, int val1, int val2, int val3, int val4)
    {
        int32_t location = glGetUniformLocation(m_RendererID, name);
        ASSERT(location != -1);
        glUniform4i(location, val1, val2, val3, val4);
    }
    void Shader::SetUniformIntArray(const char* name, int count, int* array)
    {
        int32_t location = glGetUniformLocation(m_RendererID, name);
        ASSERT(location != -1);
        glUniform1iv(location, 32, array);
    }
    void Shader::SetUniformFloat(const char* name, float val)
    {
        int32_t location = glGetUniformLocation(m_RendererID, name);
        ASSERT(location != -1);
        glUniform1f(location, val);
    }
    void Shader::SetUniformFloat(const char* name, float* ptr)
    {
        int32_t location = glGetUniformLocation(m_RendererID, name);
        ASSERT(location != -1);
        glUniform1fv(location, 1, (GLfloat*)ptr);
    }
    void Shader::SetUniformFloat2(const char* name, float val1, float val2)
    {
        int32_t location = glGetUniformLocation(m_RendererID, name);
        ASSERT(location != -1);
        glUniform2f(location, val1, val2);
    }
    void Shader::SetUniformFloat3(const char* name, float val1, float val2, float val3)
    {
        int32_t location = glGetUniformLocation(m_RendererID, name);
        ASSERT(location != -1);
        glUniform3f(location, val1, val2, val3);
    }
    void Shader::SetUniformFloat4(const char* name, float val1, float val2, float val3, float val4)
    {
        int32_t location = glGetUniformLocation(m_RendererID, name);
        ASSERT(location != -1);
        glUniform4f(location, val1, val2, val3, val4);
    }
    void Shader::SetUniformMat4(const char* name, float* val)
    {
        int32_t location = glGetUniformLocation(m_RendererID, name);
        ASSERT(location != -1);
        glUniformMatrix4fv(location, 1, GL_FALSE, val);
    }
}