#pragma once

#include <components/base_component.h>
#include <reactphysics3d/reactphysics3d.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class SpriteComponent;

class BoxCollisionComponent : public BaseComponent {
public:
    //on passe au constructeur des extents absolue, donc si on passe 60 x, la box va s'étendre de -30 à 30 autour de son centre
    //or à la création d'une box shape, rp3d demande un half-extent, on divise donc par 2 l'extent donnée à l'appel de rp3d
    explicit BoxCollisionComponent(reactphysics3d::Vector3 extent);
    ~BoxCollisionComponent() override;

    void init() override;
    void update(Window& window) override;

    void drawDebugCollisions(bool draw = true) const;

    [[nodiscard]] reactphysics3d::Collider* getCollider() const;

private:
    reactphysics3d::BoxShape* shape = nullptr;
    reactphysics3d::Collider* collider = nullptr;

    reactphysics3d::Vector3 extent;

    std::unique_ptr<SpriteComponent> sprite_debug_collision;
};

NAMEPSACE_AH2D_END;