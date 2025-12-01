#pragma once

#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class Actor;

class Controller {
public:
    Controller();
    ~Controller();

    void possess(Actor* actor);
    Actor* getPossessedActor();

private:
    Actor* possessedActor = nullptr;
};

NAMEPSACE_AH2D_END;