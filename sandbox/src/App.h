#pragma once

#include "Camera.h"
#include "OpenGLMaths.h"
#include "Shader.h"
#include "Window.h"

#include <GL/glew.h>

class App
{
  public:
    App();
    ~App() = default;

    void Init();
    void Run();
    void CleanUp();

    inline Camera &GetCamera()
    {
        return m_Camera;
    }

  private:
    void CreateTriangle();
    void SetupCallbacks();

    void UpdateUniforms();

    static void OnInterfaceUpdate();
    static void OnWindowResize(uint32_t width, uint32_t height);

    ae::WindowDesc m_WindowDesc;
    ae::Window m_Window;

    Shader m_Shader;
    Camera m_Camera;

    GLuint m_VAO;
    GLuint m_VBO;

    GLint m_ProjLoc;
    GLint m_CamLoc;

    uint32_t m_Width;
    uint32_t m_Height;

    // Static instance pointer for callbacks
    static App *s_Instance;
};
