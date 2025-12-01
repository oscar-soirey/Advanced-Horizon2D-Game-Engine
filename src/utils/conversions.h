#pragma once

#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>
#include <ui/base_widget.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

using namespace reactphysics3d;

//glm - Vector (reactphysics)
inline glm::vec3 vector3_to_glmvec3(Vector3 vec3)
{
    return {vec3.x, vec3.y, vec3.z};
}

inline Vector3 glmvec3_to_vector3(glm::vec3 vec3)
{
    return {vec3.x, vec3.y, vec3.z};
}

inline Quaternion float_to_quaternion(float angle)
{
    float half = angle / 2.0f;
    Quaternion q(0.0f, 0.0f, std::sin(half), std::cos(half));
    if (q.isValid())
        return {0.0f, 0.0f, std::sin(half), std::cos(half)};
    else
    {
        //si la rotation est invalide, on retourne un rotation d'angle 0
        return {0, 0, 0,1};
    }
}

inline float quaternion_to_float(const Quaternion& q)
{
    if (q.isValid())
        return 2.0f * std::atan2(q.z, q.w);
    //si il est invalide, on retourne un angle de 0.0f
    return 0.0f;
}

namespace pivot
{
    static const std::unordered_map<std::string, pivot_t> map =
    {
        {"PIVOT_CENTER", PIVOT_CENTER},
        {"PIVOT_TOP_LEFT", PIVOT_TOP_LEFT},
        {"PIVOT_TOP_RIGHT", PIVOT_TOP_RIGHT},
        {"PIVOT_BOTTOM_LEFT", PIVOT_BOTTOM_LEFT},
        {"PIVOT_BOTTOM_RIGHT", PIVOT_BOTTOM_RIGHT}
    };

    inline pivot_t toString(const std::string& value)
    {
        auto it = map.find(value);
        return it != map.end() ? it->second : PIVOT_CENTER;
    }
}

NAMEPSACE_AH2D_END;