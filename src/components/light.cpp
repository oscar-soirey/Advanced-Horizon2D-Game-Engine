#include "light.h"

#include <rendering/renderer.h>

NAMEPSACE_AH2D_START;

LightComponent::LightComponent(Render& render, glm::vec3 loc, glm::vec3 color, float attenuation): color(color), attenuation(attenuation), render(render)
{
    setRelativeLocation(loc);
    render.addLight(this);
}

void LightComponent::setRelativeLocation(glm::vec3 newLoc)
{
    //pour changer la position de la light on doit complètement supprimer la light du render et la re-ajouter avec la nouvelle position
    render.delLight(this);
    BaseComponent::setRelativeLocation(newLoc);
    render.addLight(this);
}

NAMEPSACE_AH2D_END;