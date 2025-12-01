#include "character_movement.h"
#include <core/window.h>
#include <actor.h>
#include <character.h>
#include <utils/log.h>

NAMEPSACE_AH2D_START;

CharacterMovementComponent::CharacterMovementComponent(Movement defaultMovement): movement_(defaultMovement) {}

void CharacterMovementComponent::update(Window& window)
{
    float dt = window.getDeltaTime();
    //si l'utilisateur n'appuie pas sur la touche de mouvement X, on interp jusqu'a ce que la velocity soit égale à 0
    if (!is_movingX && parent->xVelocity != 0.0f)  //faire en sorte que quand la vélocité est trop basse (du genre 0.0001), la mettre a 0 directement
        moveX(dt, D_NONE);
}

void CharacterMovementComponent::setMovementValues(Movement newMovement)
{
    movement_ = newMovement;
}

Movement CharacterMovementComponent::getMovementValues()
{
    return movement_;
}


void CharacterMovementComponent::moveX(double dt, float direction)
{
    is_movingX = true;
    auto parent_character = dynamic_cast<Character*>(parent);
    if (!parent_character)
    {
        LOG_ERROR("Trying to use a CharacterMovementComponent on non derived character class");
        return;
    }

    // Vitesse cible selon la direction
    float targetVelocity = direction * movement_.max_xSpeed;

    // Réduction si en l'air
    float currentAirControl = 1.0f;
    if (parent_character->isFalling())
        currentAirControl = movement_.air_control;

    //on prend acceleration rate, si on accélère, autrement on prend deceleration_rate
    float currentAccelerationRate;
    if (parent->getXAcceleration() > 0)
        currentAccelerationRate = movement_.acceleration_rate;
    else
        currentAccelerationRate = movement_.deceleration_rate;

    //----interpolation exponentielle vers la cible----//
    //(simule une accélération réaliste)
    float smoothFactor = 1.0f - std::exp(-currentAccelerationRate * dt * currentAirControl);
    parent->xVelocity += (targetVelocity - parent->xVelocity) * smoothFactor;
}

void CharacterMovementComponent::stopMoveX()
{
    is_movingX = false;
}

void CharacterMovementComponent::jump()
{
    parent->yVelocity = movement_.jump_velocity;
}

NAMEPSACE_AH2D_END;