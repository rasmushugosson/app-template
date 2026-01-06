#include "pch.h"

#include "Camera.h"

Camera::Camera(const glm::vec3 &position, float fov)
    : m_AspectRatio(1.0f), m_Fov(fov), m_Position(position), m_Rotation(0.0f), m_ProjectionMatrix(1.0f),
      m_ViewMatrix(1.0f)
{
    UpdateProjectionMatrix();
    UpdateViewMatrix();
}

void Camera::UpdateProjectionMatrix()
{
    m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, 0.1f, 100.0f);
}

void Camera::UpdateViewMatrix()
{
    // Build rotation matrix from Euler angles (pitch, yaw, roll)
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, glm::radians(-m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
    rotation = glm::rotate(rotation, glm::radians(-m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
    rotation = glm::rotate(rotation, glm::radians(-m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll

    // Translation (negative because we move the world, not the camera)
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), -m_Position);

    m_ViewMatrix = rotation * translation;
}
