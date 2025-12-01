#pragma once

#include <actor.h>
#include <core/reflection.h>
#include <components/sprite.h>
#include <components/character_movement.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class CapsuleCollisionComponent;
class LineTraceComponent;

class Character: public Actor {
public:
    std::string mainSpritePath;
    int zOrder;

    //collisions
    float capsule_radius;
    float capsule_height;
    unsigned short capsule_collision_channel;
    float max_step_height;

    Character() : Actor()
    {
        HPROPERTY(mainSpritePath, Access::Exposed);
        HPROPERTY(zOrder, Access::Exposed);
        HPROPERTY(capsule_radius, Access::Exposed);
        HPROPERTY(capsule_height, Access::Exposed);
        HPROPERTY(capsule_collision_channel, Access::Exposed);  //faut ajouter ca sinon ca va crash !!!
        HPROPERTY(max_step_height, Access::Exposed);
    }
    void init(Render& render) override;
    void update(Window &window) override;

    [[nodiscard]] CharacterMovementComponent* getMovementComponent() const;
    bool isFalling() const;

    void drawDebugTraces(bool draw = true);

protected:
    CharacterMovementComponent* movement_component = nullptr;
    CapsuleCollisionComponent* capsule_component = nullptr;

    bool jumping = false;

private:
    void addLineTracesMacro();

    bool is_falling;
    //map : string, map<string, comp_ptr> pour pouvoir appeller comme ca traces_components["left"]["bottom"] par exemple
    std::map<std::string, std::map<std::string, LineTraceComponent*>> traces_components;
};
HCLASS(Character);

NAMEPSACE_AH2D_END;