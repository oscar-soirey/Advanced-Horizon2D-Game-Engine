#include "al_audio_utils.h"

#include "AL/al.h"
#include <glm/glm.hpp>

NAMEPSACE_AH2D_START;

namespace
{
    glm::vec2 listenerPos;
}

namespace ALAudioUtils
{
    void setListenerPos(glm::vec2 newPos)
    {
        listenerPos = newPos;
        alListener3f(AL_POSITION, listenerPos.x, listenerPos.y, 0.0f);
    }

    glm::vec2 getListenerPos()
    {
        return listenerPos;
    }
}

NAMEPSACE_AH2D_END;