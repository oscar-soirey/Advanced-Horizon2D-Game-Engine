#include "al_audio_buffer.h"

#include <fstream>
#include <iostream>

NAMEPSACE_AH2D_START;

static bool LoadWavFile(const std::string& filename, ALenum& format, ALvoid*& data, ALsizei& size, ALsizei& freq)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;

    char header[44];
    file.read(header, 44);
    short audioFormat = *reinterpret_cast<short*>(&header[20]);
    short channels = *reinterpret_cast<short*>(&header[22]);
    int sampleRate = *reinterpret_cast<int*>(&header[24]);
    short bitsPerSample = *reinterpret_cast<short*>(&header[34]);
    int dataSize = *reinterpret_cast<int*>(&header[40]);

    if (audioFormat != 1) return false; // PCM

    if (channels == 1)
        format = (bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    else
        format = (bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;

    freq = sampleRate;
    size = dataSize;
    data = new char[dataSize];
    file.read(static_cast<char*>(data), dataSize);
    return true;
}


ALAudioBuffer::ALAudioBuffer(const std::string &path)
{
    alGenBuffers(1, &bufferID);

    ALenum format;
    ALvoid* data;
    ALsizei size, freq;

    if (!LoadWavFile(path, format, data, size, freq))
    {
        std::cerr << "Error while loading Wav file : " << path << std::endl;
        return;
    }

    alBufferData(bufferID, format, data, size, freq);
    delete[] static_cast<char*>(data);
}

ALAudioBuffer::~ALAudioBuffer()
{
    alDeleteBuffers(1, &bufferID);
}

ALuint ALAudioBuffer::GetBufferID() const
{
    return bufferID;
}

NAMEPSACE_AH2D_END;