#pragma once

#include "Window.h"

class App
{
  public:
    App();
    ~App() = default;

    void Init();
    void Run();
    void CleanUp();

  private:
    ae::WindowDesc m_WindowDesc;
    ae::Window m_Window;
};
