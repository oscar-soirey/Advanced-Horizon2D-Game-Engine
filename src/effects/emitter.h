#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <actor.h>
#include <core/reflection.h>
#include <rendering/renderer.h>
#include <components/base_component.h>
#include <reactphysics3d/reactphysics3d.h>

#include <utils/time.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

using namespace reactphysics3d;
using collision_channel = uint16_t;

class EmitterParticle : public BaseComponent {
public:
    EmitterParticle(Render& render, const std::string& sprite_packed_path, const std::string& collision_shape, float gravity_scale, int zOrder, Vector3 box_extent, float sphere_radius, collision_channel particle_channel, collision_channel collide_with_channel);
    ~EmitterParticle() override;

    void init() override;
    void update(Window &window) override;

    RigidBody* getRigidBody();

    void setRelativeLocation(glm::vec3 newLoc) override;
    void setRelativeRotation(float newRotation) override;
    void setRelativeScale(glm::vec2 newScale) override;

    void drawDebugCollisions(bool draw);

private:
    Render& render;
    std::string sprite_packed_path;
    std::string collision_shape;
    float gravity_scale;
    int zOrder;

    std::unique_ptr<SpriteComponent> sprite_component;
    std::unique_ptr<SpriteComponent> debug_sprite_component;

    RigidBody* rigid_body = nullptr;
    BoxShape* box_shape = nullptr;
    SphereShape* sphere_shape = nullptr;
    Collider* collider = nullptr;

    Vector3 box_extent;
    float sphere_radius;

    collision_channel particle_channel;
    collision_channel collide_with_channel;
};


class Emitter : public Actor {
public:
    //structure de l'emitter
    std::string sprite_packed_path;
    float lifespan;     //le temps de vie des particules
    int particles_num;  //le nombre de particules à spawn
    int zOrder = 0;
    //collisions -->
        std::string collision_shape = "sphere";  //les modes de collision sont "box" et "sphere", tout autre nom est ignoré
        //taille des collisions pour box et pour shpere
        Vector3 box_collision_extent{1,1,1};
        float sphere_collision_radius = 1.0f;
        collision_channel particle_channel = 0x0000;
        collision_channel collide_with_channel = 0x0000;
    //transform  -->
        glm::vec3 relative_default_loc{0};
        float relative_min_rot = 0.0f;  //rot is angle in randians
        float relative_max_rot = 0.0f;
        glm::vec2 relative_min_scale{1};
        glm::vec2 relative_max_scale{1};
    //mouvement  -->
        float min_velocity = 1.0f;
        float max_velocity = 1.0f;
        glm::vec2 min_velocity_direction{-1.0f, -1.0f};
        glm::vec2 max_velocity_direction{1.0f, 1.0f};
        float gravity_scale = 1.0f;

    Emitter();
    void init(Render &render) override;
    void update(Window &window) override;

    void drawDebugCollisions(bool draw = true);

private:
    std::vector<EmitterParticle*> particles;
    Timer life_timer{0.0f, [](){}}; //initialisation par défaut
};

NAMEPSACE_AH2D_END;