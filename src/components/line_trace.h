#pragma once

#include <memory>

#include <components/base_component.h>
#include <components/sprite.h>

#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/ground_raycast_callback.hpp>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class LineTraceComponent : public BaseComponent {
public:
    LineTraceComponent(glm::vec3 relative_start, glm::vec3 relative_end, unsigned short collision_channel = 0xFFFF);

    void init() override;
    void update(Window &window) override;

    void drawDebug(bool draw = true);

    void cast_ray();
    [[nodiscard]] bool get_hit() const;
    [[nodiscard]] GroundRaycastCallback::HitInfos getHitInfos() const;

private:
    //average of world position
    Vector3 avg;

    glm::vec3 relative_start;
    glm::vec3 relative_end;

    glm::vec3 world_start;
    glm::vec3 world_end;

    unsigned short collision_channel;

    GroundRaycastCallback::HitInfos hit_infos;

    std::unique_ptr<SpriteComponent> ray_debug;
};

NAMEPSACE_AH2D_END;