#include "pch.h"

#include "App.h"
#include "DearImGui.h"
#include "Log.h"
#include "Window.h"

static const char *s_VertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;

uniform mat4 u_Proj;
uniform mat4 u_Cam;

out vec3 o_VertexColor;

void main()
{
    gl_Position = u_Proj * u_Cam * vec4(a_Position, 1.0);
    o_VertexColor = a_Color;
}
)";

static const char *s_FragmentShaderSource = R"(
#version 330 core
in vec3 o_VertexColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(o_VertexColor, 1.0);
}
)";

constexpr float c_DefaultFov = 45.0f;
constexpr glm::vec3 c_DefaultPosition = glm::vec3(0.0f, 0.0f, 2.5f);
constexpr glm::vec3 c_DefaultRotation = glm::vec3(0.0f);

App *App::s_Instance = nullptr;

App::App()
    : m_WindowDesc("Sandbox", 1280, 720, true, true, false, true, false, 0, true, 60, ae::WindowType::WINDOWED,
                   ae::GraphicsAPI::OPENGL),
      m_Window(m_WindowDesc), m_Shader(s_VertexShaderSource, s_FragmentShaderSource),
      m_Camera(c_DefaultPosition, c_DefaultFov), m_VAO(0), m_VBO(0), m_ProjLoc(-1), m_CamLoc(-1), m_Width(1280),
      m_Height(720)
{
    s_Instance = this;
}

void App::Init()
{
    m_Window.Create();

    m_Shader.Init();
    CreateTriangle();
    SetupCallbacks();

    // Get uniform locations
    m_ProjLoc = glGetUniformLocation(m_Shader.GetProgram(), "u_Proj");
    m_CamLoc = glGetUniformLocation(m_Shader.GetProgram(), "u_Cam");
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

        UpdateUniforms();

        GL_CHECK(glUseProgram(m_Shader.GetProgram()));
        GL_CHECK(glBindVertexArray(m_VAO));
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3));

        m_Window.Update();
    }
}

void App::CleanUp()
{
    s_Instance = nullptr;

    GL_CHECK(glDeleteVertexArrays(1, &m_VAO));
    GL_CHECK(glDeleteBuffers(1, &m_VBO));
    m_Shader.CleanUp();

    m_Window.Destroy();
}

void App::CreateTriangle()
{
    // Vertex data: position (x, y, z) and color (r, g, b)
    // clang-format off
    std::array<float, 18> vertices = {
        // Position          // Color
         0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Top vertex (red)
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom left (green)
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Bottom right (blue)
    };
    // clang-format on

    GL_CHECK(glGenVertexArrays(1, &m_VAO));
    GL_CHECK(glGenBuffers(1, &m_VBO));

    GL_CHECK(glBindVertexArray(m_VAO));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW));

    // Position attribute (location = 0)
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), ae::BufferOffset(0)));
    GL_CHECK(glEnableVertexAttribArray(0));

    // Color attribute (location = 1)
    GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), ae::BufferOffset(3 * sizeof(float))));
    GL_CHECK(glEnableVertexAttribArray(1));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CHECK(glBindVertexArray(0));
}

void App::SetupCallbacks()
{
    m_Window.SetOnInterfaceUpdateCB(OnInterfaceUpdate);
    m_Window.SetOnWindowResizeCB(OnWindowResize);
}

void App::UpdateUniforms()
{
    GL_CHECK(glUseProgram(m_Shader.GetProgram()));
    GL_CHECK(glUniformMatrix4fv(m_ProjLoc, 1, GL_FALSE, glm::value_ptr(m_Camera.GetProjectionMatrix())));
    GL_CHECK(glUniformMatrix4fv(m_CamLoc, 1, GL_FALSE, glm::value_ptr(m_Camera.GetViewMatrix())));
}

void App::OnInterfaceUpdate()
{
    if (!s_Instance)
    {
        return;
    }

    App &app = *s_Instance;
    std::shared_ptr<ae::Context> pContext = app.m_Window.GetContext().lock();

    // Camera Controls Window
    ImGui::Begin("Camera");

    Camera &camera = app.GetCamera();

    float fov = camera.GetFov();
    if (ImGui::SliderFloat("FOV", &fov, 10.0f, 120.0f, "%.1f"))
    {
        camera.SetFov(fov);
    }

    glm::vec3 pos = camera.GetPosition();
    glm::vec3 rot = camera.GetRotation();

    ImGui::Separator();
    ImGui::Text("Position");
    ImGui::DragFloat("X##pos", &pos.x, 0.01f, -10.0f, 10.0f, "%.2f");
    ImGui::DragFloat("Y##pos", &pos.y, 0.01f, -10.0f, 10.0f, "%.2f");
    ImGui::DragFloat("Z##pos", &pos.z, 0.01f, -10.0f, 10.0f, "%.2f");

    ImGui::Separator();
    ImGui::Text("Rotation (degrees)");
    ImGui::DragFloat("Pitch", &rot.x, 0.5f, -180.0f, 180.0f, "%.1f");
    ImGui::DragFloat("Yaw", &rot.y, 0.5f, -180.0f, 180.0f, "%.1f");
    ImGui::DragFloat("Roll", &rot.z, 0.5f, -180.0f, 180.0f, "%.1f");

    camera.SetOrientation(pos, rot);

    ImGui::Separator();
    if (ImGui::Button("Reset Camera"))
    {
        camera.SetFov(c_DefaultFov);
        camera.SetOrientation(c_DefaultPosition, c_DefaultRotation);
    }

    ImGui::End();

    // Info Window
    ImGui::Begin("Info");

    ImGui::Text("Resolution");
    ImGui::Text("  %u x %u", app.m_Width, app.m_Height);
    ImGui::Text("  Aspect: %.3f", static_cast<float>(app.m_Width) / static_cast<float>(app.m_Height));

    ImGui::Separator();
    ImGui::Text("Frame Stats");
    ImGui::Text("  FPS: %.0f", app.m_Window.GetFps());
    ImGui::Text("  Frame Time: %.2f ms", app.m_Window.GetAverageFrameTime() * 1000.0);
    ImGui::Text("  Frame Duration: %.2f ms", app.m_Window.GetAverageFrameDuration() * 1000.0);

    if (pContext)
    {
        ImGui::Separator();
        ImGui::Text("Graphics");
        ImGui::Text("  API: %s", pContext->GetGraphicsAPI().c_str());
        ImGui::Text("  Version: %s", pContext->GetGraphicsVersion().c_str());
        ImGui::Text("  GPU: %s", pContext->GetGraphicsCard().c_str());
        ImGui::Text("  Vendor: %s", pContext->GetGraphicsVendor().c_str());
    }

    ImGui::End();
}

void App::OnWindowResize(uint32_t width, uint32_t height)
{
    if (!s_Instance || width == 0 || height == 0)
    {
        return;
    }

    s_Instance->m_Width = width;
    s_Instance->m_Height = height;

    Camera &camera = s_Instance->GetCamera();
    camera.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));

    GL_CHECK(glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height)));
}
