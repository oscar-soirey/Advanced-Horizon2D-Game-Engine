#include "line_trace.h"

#include <actor.h>
#include <rendering/renderer.h>

#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/ground_raycast_callback.hpp>

#include <utils/log.h>
#include <utils/conversions.h>

NAMEPSACE_AH2D_START;

using namespace reactphysics3d;

LineTraceComponent::LineTraceComponent(glm::vec3 relative_start, glm::vec3 relative_end, unsigned short collision_channel) : relative_start(relative_start), relative_end(relative_end),
    world_start(relative_start+getWorldLocation()), world_end(relative_end+getWorldLocation()), collision_channel(collision_channel)
    {}

void LineTraceComponent::init()
{
    //debug visuel
    ray_debug = std::make_unique<SpriteComponent>(*parent->getRenderObject(), "engine/raycast.png", 10000);
    //position
    //on passe par un Vector3 pour faire l'opération division (non disponible avec glm::vec3)
    avg = glmvec3_to_vector3(world_start + world_end)/2;
    ray_debug->setRelativeLocation(vector3_to_glmvec3(avg));  //on fait la moyenne entre les deux positions
    //scale
    //--> direction * distance = end - start
    auto scale3 = world_end - world_start;
    //ray_debug->setRelativeScale(glm::vec2(std::max(1.0f, scale3.x), std::max(1.0f, scale3.y)));
    ray_debug->visible = false;
}

void LineTraceComponent::update(Window &window)
{
    glm::vec3 loc = vector3_to_glmvec3(avg) + getWorldLocation();
    ray_debug->setRelativeLocation(loc);
    BaseComponent::update(window);
}

void LineTraceComponent::drawDebug(bool draw)
{
    ray_debug->visible = draw;
}

void LineTraceComponent::cast_ray()
{
    world_start = relative_start+getWorldLocation();
    world_end = relative_end+getWorldLocation();

    Ray ray(glmvec3_to_vector3(world_start), glmvec3_to_vector3(world_end));

    GroundRaycastCallback callback(collision_channel);
    parent->getRenderObject()->getPhysicsWorld()->raycast(ray, &callback);
    hit_infos = callback.hit_infos;
}

bool LineTraceComponent::get_hit() const
{
    return hit_infos.hit;
}

GroundRaycastCallback::HitInfos LineTraceComponent::getHitInfos() const
{
    return hit_infos;
}

NAMEPSACE_AH2D_END;