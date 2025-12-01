#pragma once

#include <string>

#include <AL/al.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class ALAudioBuffer {
public:
    ALAudioBuffer(const std::string& path);
    ~ALAudioBuffer();

    [[nodiscard]] ALuint GetBufferID() const;

private:
    ALuint bufferID;

};

NAMEPSACE_AH2D_END;