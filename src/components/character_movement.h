#pragma once

#include <components/base_component.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

//Character movement gravity constant
#define GRAVITY 9.81f
//Direction
#define D_RIGHT 1.0f
//Direction
#define D_LEFT (-1.0f)
//Direction
#define D_NONE 0.0f

struct Movement {
    float gravity_multiplier;
    float jump_velocity;
    float max_xSpeed;
    float acceleration_rate;
    float deceleration_rate;
    float air_control;
    float max_step_height;
};

class CharacterMovementComponent : public BaseComponent {
public:
    explicit CharacterMovementComponent(Movement defaultMovement);
    void update(Window& window) override;

    void setMovementValues(Movement newMovement);
    Movement getMovementValues();

    //direction is RIGHT or LEFT (bool constants)
    void moveX(double dt, float direction);
    void stopMoveX();
    void jump();

protected:
    Movement movement_;

private:
    bool is_movingX = false;
};

NAMEPSACE_AH2D_END;