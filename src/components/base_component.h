#pragma once

#include <glm/glm.hpp>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class Actor;
class Window;
class Render;

class BaseComponent {
public:
    virtual ~BaseComponent();
    virtual void update(Window& window);

    virtual void init();

    void setParent(Actor* newParent);
    Actor* getParent();

    glm::vec3 getRelativeLocation();
    float getRelativeRotation();
    glm::vec2 getRelativeScale();
    glm::vec3 getWorldLocation();
    float getWorldRotation();
    glm::vec2 getWorldScale();

    virtual void setRelativeLocation(glm::vec3 newLoc);
    virtual void setRelativeRotation(float newRotation);
    virtual void setRelativeScale(glm::vec2 newScale);

protected:
    Actor* parent = nullptr;

    glm::vec3 relativeLoc = glm::vec3(0.0f);
    float relativeRot = 0.0f;
    glm::vec2 relativeScale = glm::vec2(1.0f);
};

NAMEPSACE_AH2D_END;