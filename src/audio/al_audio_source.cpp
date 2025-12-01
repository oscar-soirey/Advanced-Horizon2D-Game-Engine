#include "al_audio_source.h"
#include "al_audio_utils.h"

NAMEPSACE_AH2D_START;

ALAudioSource::ALAudioSource()
{
    alGenSources(1, &sourceID);
}

ALAudioSource::~ALAudioSource()
{
    alDeleteSources(1, &sourceID);
}

void ALAudioSource::play() const
{
    //set les variables nécéssaires : attenuation et position ( l'attenuation dépend de la position du son ! )
    alSource3f(sourceID, AL_POSITION, position.x, position.y, 0.0f);

    alSourcef(sourceID, AL_REFERENCE_DISTANCE, attenuation.threshold);
    alSourcef(sourceID, AL_MAX_DISTANCE, attenuation.maxDistance);
    alSourcef(sourceID, AL_ROLLOFF_FACTOR, attenuation.fallofFactor);

    //play le son
    alSourcePlay(sourceID);

}

void ALAudioSource::play2D() const
{
    //pareil mais il ne faut pas d'attenuation
    alSource3f(sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);

    alSourcef(sourceID, AL_REFERENCE_DISTANCE, 1000000.0f);
    alSourcef(sourceID, AL_MAX_DISTANCE, 1000000.0f);
    alSourcef(sourceID, AL_ROLLOFF_FACTOR, 0.0f);

    alSourcePlay(sourceID);
}

void ALAudioSource::pause() const
{
    alSourcePause(sourceID);
}

void ALAudioSource::stop() const
{
    alSourceStop(sourceID);
}

void ALAudioSource::setBuffer(ALuint buffer) const
{
    alSourcei(sourceID, AL_BUFFER, buffer);
    alSourcei(sourceID, AL_DISTANCE_MODEL, AL_INVERSE_DISTANCE_CLAMPED);
}

void ALAudioSource::setLooping(bool loop) const
{
    alSourcei(sourceID, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void ALAudioSource::setPosition(glm::vec2 pos)
{
    position = pos;
}

void ALAudioSource::setAttenuation(Attenuation att)
{
    attenuation = att;
}

NAMEPSACE_AH2D_END;