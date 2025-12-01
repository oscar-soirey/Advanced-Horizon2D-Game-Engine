#include "controller.h"
#include <actor.h>

NAMEPSACE_AH2D_START;

Controller::Controller() = default;
Controller::~Controller() = default;

void Controller::possess(Actor *actor)
{
    if (possessedActor) possessedActor->unpossess();
    if (actor)
    {
        actor->possess();
        possessedActor = actor;
    }
}

Actor *Controller::getPossessedActor()
{
    return possessedActor;
}

NAMEPSACE_AH2D_END;