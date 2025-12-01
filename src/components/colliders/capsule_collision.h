#pragma once

#include <components/base_component.h>
#include <components/sprite.h>

#include <reactphysics3d/reactphysics3d.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class CapsuleCollisionComponent : public BaseComponent {
public:
    CapsuleCollisionComponent(float radius, float height);

    void init() override;
    void update(Window &window) override;

    [[nodiscard]] reactphysics3d::Collider* getCollider() const;

    void drawDebugCollisions(bool draw = true) const;

private:
    reactphysics3d::CapsuleShape* capsuleShape = nullptr;
    reactphysics3d::Collider* collider = nullptr;

    float radius;
    float height;

    std::unique_ptr<SpriteComponent> sprite_debug_collision;
};

NAMEPSACE_AH2D_END;