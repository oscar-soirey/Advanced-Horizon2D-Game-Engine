#include "base_component.h"

#include <actor.h>

NAMEPSACE_AH2D_START;

BaseComponent::~BaseComponent() = default;

void BaseComponent::update(Window &window) {}
void BaseComponent::init() {}


void BaseComponent::setParent(Actor *newParent)
{
    parent = newParent;
}

Actor *BaseComponent::getParent()
{
    return parent;
}


glm::vec3 BaseComponent::getRelativeLocation()
{
    return relativeLoc;
}

float BaseComponent::getRelativeRotation()
{
    return relativeRot;
}

glm::vec2 BaseComponent::getRelativeScale()
{
    return relativeScale;
}

glm::vec3 BaseComponent::getWorldLocation()
{
    if (parent)
        return parent->getActorLocation() + relativeLoc;
    return relativeLoc;
}

float BaseComponent::getWorldRotation()
{
    if (parent)
        return parent->getActorRotation() + relativeRot;
    return relativeRot;
}

glm::vec2 BaseComponent::getWorldScale()
{
    if (parent)
        return parent->getActorScale() * relativeScale;
    return relativeScale;
}

void BaseComponent::setRelativeLocation(glm::vec3 newLoc)
{
    relativeLoc = newLoc;
}

void BaseComponent::setRelativeRotation(float newRotation)
{
    relativeRot = newRotation;
}

void BaseComponent::setRelativeScale(glm::vec2 newScale)
{
    relativeScale = newScale;
}


NAMEPSACE_AH2D_END;