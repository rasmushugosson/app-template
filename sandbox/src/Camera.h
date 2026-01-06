#pragma once

#include "OpenGLMaths.h"

class Camera
{
  public:
    Camera(const glm::vec3 &position, float fov);
    ~Camera() = default;

    inline float GetAspectRatio() const
    {
        return m_AspectRatio;
    }

    inline void SetAspectRatio(float aspectRatio)
    {
        m_AspectRatio = aspectRatio;
        UpdateProjectionMatrix();
    }

    inline float GetFov() const
    {
        return m_Fov;
    }

    inline void SetFov(float fov)
    {
        m_Fov = fov;
        UpdateProjectionMatrix();
    }

    inline const glm::vec3 &GetPosition() const
    {
        return m_Position;
    }

    inline const glm::vec3 &GetRotation() const
    {
        return m_Rotation;
    }

    inline void SetOrientation(const glm::vec3 &position, const glm::vec3 &rotation)
    {
        m_Position = position;
        m_Rotation = rotation;

        UpdateViewMatrix();
    }

    inline const glm::mat4 &GetProjectionMatrix() const
    {
        return m_ProjectionMatrix;
    }

    inline const glm::mat4 &GetViewMatrix() const
    {
        return m_ViewMatrix;
    }

  private:
    void UpdateProjectionMatrix();
    void UpdateViewMatrix();

  private:
    float m_AspectRatio;
    float m_Fov;

    glm::vec3 m_Position;
    glm::vec3 m_Rotation; // Pitch, Yaw, Roll in degrees

    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;
};
