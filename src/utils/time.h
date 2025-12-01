#pragma once

#include <functional>
#include <utility>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

struct Timer {
public:
    Timer(float delay, std::function<void()> func) : delay(delay), accumulator(0.0f), function(std::move(func)) {}
    void update(float deltaTime)
    {
        accumulator += deltaTime;
        if (accumulator >= delay)
        {
            reset();
            function();
        }
    }
    void reset()
    {
        accumulator = 0.0f;
    }

private:
    float delay;  //temps de délai
    float accumulator;  //temps écoulé depuis le début du délai
    std::function<void()> function;
};

NAMEPSACE_AH2D_END;