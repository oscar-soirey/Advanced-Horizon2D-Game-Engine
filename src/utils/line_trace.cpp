#include "line_trace.h"

#include <rendering/renderer.h>
#include <components/sprite.h>
#include <reactphysics3d/ground_raycast_callback.hpp>
#include <utils/conversions.h>

NAMEPSACE_AH2D_START;

LineTrace::LineTrace(Render& render, glm::vec3 start, glm::vec3 end, unsigned short collision_channel)
{
    Ray ray(glmvec3_to_vector3(start), glmvec3_to_vector3(end));

    GroundRaycastCallback callback(collision_channel);
    render.getPhysicsWorld()->raycast(ray, &callback);

    hit_infos = callback.hit_infos;
}

LineTrace::~LineTrace() = default;

bool LineTrace::get_hit() const
{
    return hit_infos.hit;
}

GroundRaycastCallback::HitInfos LineTrace::getHitInfos() const
{
    return hit_infos;
}


NAMEPSACE_AH2D_END;