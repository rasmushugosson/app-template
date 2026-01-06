#pragma once

#include "OpenGL.h"

#include <string>
#include <string_view>

class Shader
{
  public:
    Shader(std::string_view vertexSource, std::string_view fragmentSource);
    ~Shader() = default;

    void Init();
    void CleanUp();

    inline GLuint GetProgram() const
    {
        return m_Program;
    }

  private:
    GLuint CompileShader(GLenum type, const char *source);
    GLuint CreateProgram();

  private:
    std::string m_VertexSource;
    std::string m_FragmentSource;
    GLuint m_Program;
};
