#include "pch.h"

#include "App.h"
#include "Log.h"
#include "Window.h"

App::App()
    : m_WindowDesc("Sandbox", 1280, 720, true, true, false, true, false, false, 0, true, 60, ae::GraphicsAPI::OPENGL),
      m_Window(m_WindowDesc)
{
}

void App::Init()
{
    m_Window.Create();
}

void App::Run()
{
    while (!m_Window.ShouldClose())
    {
        if (m_Window.GetKeyboard().IsKeyPressed(ae::Key::ESCAPE))
        {
            m_Window.Close();
        }

        m_Window.SetActive();
        m_Window.Clear();

        ae::DateTime::Wait(0.01);

        m_Window.Update();
    }
}

void App::CleanUp()
{
    m_Window.Destroy();
}
