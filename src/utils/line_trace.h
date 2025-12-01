#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/ground_raycast_callback.hpp>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

using namespace reactphysics3d;

class SpriteComponent;
class Render;

class LineTrace {
public:
    LineTrace(Render& render, glm::vec3 start, glm::vec3 end, unsigned short collision_channel);
    ~LineTrace();

    [[nodiscard]] bool get_hit() const;
    [[nodiscard]] GroundRaycastCallback::HitInfos getHitInfos() const;

private:
    GroundRaycastCallback::HitInfos hit_infos;
};


NAMEPSACE_AH2D_END;