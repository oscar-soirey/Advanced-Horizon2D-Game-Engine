#pragma once

#include <ah2d/actor.h>
#include <ah2d/components/colliders/box_collision.h>
#include <ah2d/core/reflection.h>
#include "collision_channel.h"
#include <ah2d/core/h2d_namespace.h>

USING_NAMESPACE_AH2D;

using namespace reactphysics3d;

class Platform : public Actor {
public:
    Platform() : Actor()
    {
    }

    void init(Render &render) override
    {
        Actor::init(render);
        addComponent<BoxCollisionComponent>("box_collision", Vector3(60,60,10));
        auto box_collision = dynamic_cast<BoxCollisionComponent*>(getComponentByID("box_collision"));

        box_collision->drawDebugCollisions();

        getRigidBody()->setType(BodyType::STATIC);
        box_collision->getCollider()->setCollisionCategoryBits(WORLD_STATIC);
        box_collision->getCollider()->setCollideWithMaskBits(ALL);
        box_collision->getCollider()->getMaterial().setBounciness(0.0f);
    }
    void update(Window &window) override
    {
        Actor::update(window);
    }
};
HCLASS(Platform);