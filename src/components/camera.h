#pragma once

#include <glm/glm.hpp>

#include <core/maths.h>
#include <components/base_component.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class Shader;

class CameraComponent : public BaseComponent {
public:
    CameraComponent(float fov);

    void update(const Shader& shader);
    glm::mat4 getView();

    void move(glm::vec3 movement);
    maths::Rotator getCameraRotator();
    void setCameraRotation(maths::Rotator newCameraRotation);

    float fov;

private:
    glm::mat4 view = glm::mat4(1.0f);
    maths::Rotator cameraRotation;
};

NAMEPSACE_AH2D_END;