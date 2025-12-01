#pragma once

#include <random>
#include <reactphysics3d/reactphysics3d.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

namespace maths
{
    struct Rotator{
        float yaw = 0.0f;
        float pitch = 0.0f;
        float roll = 0.0f;

        [[nodiscard]] glm::vec3 getForwardVector() const
        {
            glm::vec3 forward;
            forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            forward.y = sin(glm::radians(pitch));
            forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            return glm::normalize(forward);
        }

        //Vecteur right (perpendiculaire à forward, sur le plan horizontal)
        [[nodiscard]] glm::vec3 getRightVector() const
        {
            //L’up global est (0,1,0)
            return glm::normalize(glm::cross(getForwardVector(), glm::vec3(0.0f, 1.0f, 0.0f)));
        }

        // Vecteur up (perpendiculaire à forward et right)
        [[nodiscard]] glm::vec3 getUpVector() const
        {
            return glm::normalize(glm::cross(getRightVector(), getForwardVector()));
        }
    };

    inline float randomFloatInRange(float min, float max)
    {
        //Crée un générateur aléatoire basé sur un vrai seed (chrono, hardware, etc...)
        std::random_device rd;
        std::mt19937 gen(rd());  //Mersenne Twister (générateur de bonne qualité)

        // Crée une distribution de réels entre min et max
        std::uniform_real_distribution<float> dist(min, max);

        //Tire un nombre aléatoire dans la plage
        float value = dist(gen);

        return value;
    }


    //le nombre d'id différent possible est 2^64 soit 18 446 744 073 709 551 616
    //il y a 2.7% de chance d'avoir une collision d'ID pour 10^9 (1 milliard) tirages
    inline std::string generateRandomID(const std::string& prefix = "", const std::string& sufix = "")
    {
        static std::mt19937 rng(std::random_device{}()); // moteur aléatoire
        static std::uniform_int_distribution<unsigned long long> dist(0, 0xFFFFFFFFFFFFFFFFULL);

        unsigned long long value = dist(rng);
        return prefix + std::to_string(value) + sufix;
    }

    //smoothStep, square, easeOut, easeInOut, exp, sinEaseInOut, always with 0 <= t <= 1
    namespace interp
    {
        //interpolation en forme de s
        inline float smoothstep(float t)
        {
            return t*t*(3 - 2*t);
        }
        inline float square(float t)
        {
            return t*t;
        }
        inline float easeOut(float t)
        {
            return 1 - (1-t) * (1-t);
        }
        inline float easeInOut(float t)
        {
            return t < 0.5f ? 2*t*t : 1 - pow(-2*t + 2, 2) / 2;
        }
        inline float exp(float t)
        {
            return t == 1 ? 1 : 1 - pow(2, -10 * t);
        }
        inline float sinEaseInOut(float t)
        {
            return 0.5f * (1 - cos(t * M_PI));
        }
    }

    //transforme linéairement une valeur comprise dans un intervalle en un autre intervalle
    inline float mapRange(float value, float inMin, float inMax, float outMin, float outMax)
    {
        return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
    }

    inline bool floatInRange(float value, float range_min, float range_max, bool allow_equal = true)
    {
        if (allow_equal)
            return value >= range_min && value <= range_max;
        return value > range_min && value < range_max;
    }

    inline reactphysics3d::Quaternion float_to_quaternion(float angle)
    {
        float half = angle / 2.0f;
        reactphysics3d::Quaternion q(0.0f, 0.0f, std::sin(half), std::cos(half));
        if (q.isValid())
            return { 0.0f, 0.0f, std::sin(half), std::cos(half) };
        else
        {
            //si la rotation est invalide, on retourne un rotation d'angle 0
            return { 0, 0, 0,1 };
        }
    }

    inline float quaternion_to_float(const reactphysics3d::Quaternion& q)
    {
        if (q.isValid())
            return 2.0f * std::atan2(q.z, q.w);
        //si il est invalide, on retourne un angle de 0.0f
        return 0.0f;
    }
}

NAMEPSACE_AH2D_END;