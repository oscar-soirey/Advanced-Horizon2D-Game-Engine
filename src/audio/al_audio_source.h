#pragma once

#include <AL/al.h>
#include <glm/glm.hpp>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

struct Attenuation {
    float threshold;
    float maxDistance;
    float fallofFactor;
};

class ALAudioSource {
public:
    ALAudioSource();
    ~ALAudioSource();

    void play() const;
    void play2D() const;
    void pause() const;
    void stop() const;
    void setBuffer(ALuint buffer) const;
    void setLooping(bool loop) const;
    void setPosition(glm::vec2 pos);
    void setAttenuation(Attenuation att);

private:
    ALuint sourceID;
    glm::vec2 position = glm::vec2(0.0f);
    Attenuation attenuation;

};

NAMEPSACE_AH2D_END;