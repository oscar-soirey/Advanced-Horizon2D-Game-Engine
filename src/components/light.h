#pragma once

#include <glm/glm.hpp>
#include <components/base_component.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class Render;

class LightComponent : public BaseComponent {
public:
    LightComponent(Render& render, glm::vec3 loc, glm::vec3 color, float attenuation);

    void setRelativeLocation(glm::vec3 newLoc) override;

    glm::vec3 color;
    float attenuation;

private:
    Render& render;
};

NAMEPSACE_AH2D_END;