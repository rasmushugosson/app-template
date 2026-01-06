#include "pch.h"

#include "Shader.h"

#include <array>

Shader::Shader(std::string_view vertexSource, std::string_view fragmentSource)
    : m_VertexSource(vertexSource), m_FragmentSource(fragmentSource), m_Program(0)
{
}

void Shader::Init()
{
    m_Program = CreateProgram();
}

void Shader::CleanUp()
{
    GL_CHECK(glDeleteProgram(m_Program));

    m_Program = 0;
}

GLuint Shader::CompileShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    GL_CHECK(glShaderSource(shader, 1, &source, nullptr));
    GL_CHECK(glCompileShader(shader));

    GLint success = 0;
    GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
    if (!success)
    {
        std::array<char, 512> infoLog;
        GL_CHECK(glGetShaderInfoLog(shader, 512, nullptr, infoLog.data()));
        const char *typeName = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        AE_THROW_RUNTIME_ERROR("{} shader compilation failed: {}", typeName, infoLog);
    }

    return shader;
}

GLuint Shader::CreateProgram()
{
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, m_VertexSource.c_str());
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, m_FragmentSource.c_str());

    GLuint program = glCreateProgram();
    GL_CHECK(glAttachShader(program, vertexShader));
    GL_CHECK(glAttachShader(program, fragmentShader));
    GL_CHECK(glLinkProgram(program));

    GLint success = 0;
    GL_CHECK(glGetProgramiv(program, GL_LINK_STATUS, &success));
    if (!success)
    {
        std::array<char, 512> infoLog;
        GL_CHECK(glGetProgramInfoLog(program, 512, nullptr, infoLog.data()));
        AE_THROW_RUNTIME_ERROR("Shader program linking failed: {}", infoLog);
    }

    GL_CHECK(glDeleteShader(vertexShader));
    GL_CHECK(glDeleteShader(fragmentShader));

    return program;
}
