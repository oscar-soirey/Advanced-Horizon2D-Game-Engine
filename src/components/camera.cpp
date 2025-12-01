#include "camera.h"
#include <rendering/shader.h>

#include <glm/gtc/matrix_transform.hpp>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

CameraComponent::CameraComponent(float fov) : fov(fov)
{
}

void CameraComponent::update(const Shader& shader)
{
    shader.setMat4("view", view);
}

void CameraComponent::move(glm::vec3 movement)
{
    setRelativeLocation(getRelativeLocation() += movement);
    view = glm::translate(view, glm::vec3(-movement));
}

glm::mat4 CameraComponent::getView()
{
    return view;
}

maths::Rotator CameraComponent::getCameraRotator()
{
    return cameraRotation;
}

void CameraComponent::setCameraRotation(maths::Rotator newCameraRotation)
{
    cameraRotation = newCameraRotation;
    glm::vec3 direction;
    direction.x = cos(glm::radians(cameraRotation.yaw)) * cos(glm::radians(cameraRotation.pitch));
    direction.y = sin(glm::radians(cameraRotation.pitch));
    direction.z = sin(glm::radians(cameraRotation.yaw)) * cos(glm::radians(cameraRotation.pitch));
    direction = glm::normalize(direction);
    view = glm::lookAt(getWorldLocation(), getWorldLocation() + direction, glm::vec3(0.0f, 1.0f, 0.0f));
}


NAMEPSACE_AH2D_END;