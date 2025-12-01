#include "capsule_collision.h"

#include <actor.h>
#include <rendering/renderer.h>

NAMEPSACE_AH2D_START;

CapsuleCollisionComponent::CapsuleCollisionComponent(float radius, float height) : radius(radius), height(height) {}

void CapsuleCollisionComponent::init()
{
    auto ro = parent->getRenderObject();
    capsuleShape = ro->getPhysicsCommon()->createCapsuleShape(radius, height);
    collider = parent->getRigidBody()->addCollider(capsuleShape, reactphysics3d::Transform::identity());

    sprite_debug_collision = std::make_unique<SpriteComponent>(*parent->getRenderObject(), "engine/capsule_collision.png", 1000);
    sprite_debug_collision->setRelativeScale(glm::vec2(radius/16, height/28));  //on prend uniquement la partie du cylindre central, qui fait 280 pixels de hauteur sur l'image
    sprite_debug_collision->visible = false;
}

void CapsuleCollisionComponent::update(Window &window)
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

reactphysics3d::Collider *CapsuleCollisionComponent::getCollider() const
{
    return collider;
}

void CapsuleCollisionComponent::drawDebugCollisions(bool draw) const
{
    sprite_debug_collision->visible = draw;
}

NAMEPSACE_AH2D_END;