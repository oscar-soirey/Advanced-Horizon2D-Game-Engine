#include "box_collision.h"

#include <actor.h>
#include <rendering/renderer.h>

NAMEPSACE_AH2D_START;

BoxCollisionComponent::BoxCollisionComponent(reactphysics3d::Vector3 extent) : extent(extent) {}

BoxCollisionComponent::~BoxCollisionComponent() = default;

void BoxCollisionComponent::init()
{
    auto ro = parent->getRenderObject();
    shape = ro->getPhysicsCommon()->createBoxShape(extent/2);  //on met /2 car rp3d attend une valeur half
    collider = parent->getRigidBody()->addCollider(shape, reactphysics3d::Transform::identity());

    sprite_debug_collision = std::make_unique<SpriteComponent>(*parent->getRenderObject(), "engine/box_collision.png", 1000);
    sprite_debug_collision->setRelativeScale(glm::vec2(extent.x/10, extent.y/10));
    sprite_debug_collision->visible = false;
}

void BoxCollisionComponent::update(Window &window)
{
    //update
    if (sprite_debug_collision->visible)
    {
        glm::vec3 loc = getWorldLocation();
        float rot = getWorldRotation();
        sprite_debug_collision->setRelativeLocation(glm::vec3(loc.x, loc.y, loc.z));
        sprite_debug_collision->setRelativeRotation(rot);
    }
    BaseComponent::update(window);
}

reactphysics3d::Collider *BoxCollisionComponent::getCollider() const
{
    return collider;
}

void BoxCollisionComponent::drawDebugCollisions(bool draw) const
{
    sprite_debug_collision->visible = draw;
}

NAMEPSACE_AH2D_END;