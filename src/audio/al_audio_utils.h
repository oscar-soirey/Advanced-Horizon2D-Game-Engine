#pragma once

#include "glm/glm.hpp"
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

namespace ALAudioUtils
{
    void setListenerPos(glm::vec2 newPos);
    glm::vec2 getListenerPos();
}

NAMEPSACE_AH2D_END;