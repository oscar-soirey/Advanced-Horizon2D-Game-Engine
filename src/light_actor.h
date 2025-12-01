#pragma once

#include <actor.h>
#include <core/reflection.h>
#include <components/light.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class Light : public Actor{
public:
    void init(Render& render)
    {
        addComponent<LightComponent>("light0", render, glm::vec3(0), glm::vec3(1.0), 10000);
    }
};
HCLASS(Light);

NAMEPSACE_AH2D_END;