#pragma once

#include <ah2d/effects/emitter.h>
#include "collision_channel.h"
#include <ah2d/core/h2d_namespace.h>

USING_NAMESPACE_AH2D;

class TestEmitter : public Emitter {
public:
    TestEmitter()
    {
        //structure de l'emitter
        sprite_packed_path = "engine/particle.png";
        lifespan = 1.0f;     //la durée de vie des particules
        particles_num = 500;  //le nombre de particules à spawn
        zOrder = 10;
        //collisions -->
            collision_shape = "";  //les modes de collision sont "box" et "sphere", tout autre nom est ignoré
            sphere_collision_radius = 5;
            box_collision_extent = Vector3(10,10,10);
            particle_channel = PARTICLE;
            collide_with_channel = ALL & ~(PARTICLE | PLAYER);
        //transform  -->
            relative_default_loc = glm::vec3(0);
            relative_min_rot = 0.0f;  //rot is angle in randians
            relative_max_rot = 0.0f;
            relative_min_scale = glm::vec2(1);
            relative_max_scale = glm::vec2(1);
        //mouvement  -->
            min_velocity = 50.0f;
            max_velocity = 60.0f;
            min_velocity_direction = glm::vec2(-1, -1);
            max_velocity_direction = glm::vec2(1, 1);
            gravity_scale = 1.0f;
    }
};
HCLASS(TestEmitter);