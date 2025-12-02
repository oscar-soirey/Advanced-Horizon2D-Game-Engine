#pragma once

#include <ah2d/actor.h>
#include <ah2d/components/colliders/box_collision.h>
#include <ah2d/core/reflection.h>
#include "collision_channel.h"
#include <ah2d/core/h2d_namespace.h>

#include "components/sprite.h"

USING_NAMESPACE_AH2D;

using namespace reactphysics3d;

class SpriteTest : public Actor {
public:
    SpriteComponent* sprite_component = nullptr;

    SpriteTest() : Actor()
    {
    }

    void init(Render &render) override
    {
        Actor::init(render);
        addComponent<SpriteComponent>("sprite", render, "engine/box_collision.png", 10);
        sprite_component = dynamic_cast<SpriteComponent*>(getComponentByID("sprite"));

        getRigidBody()->setType(BodyType::STATIC);
    }

    void update(Window &window) override
    {
        Actor::update(window);
        sprite_component->setPivot(glm::vec2(50,50));
    }
};
HCLASS(SpriteTest);