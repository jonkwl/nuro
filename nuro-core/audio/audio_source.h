#pragma once

#include <ecs/components.h>

class AudioClip;

namespace AudioSource {

    // Set gain (volume)
    void setGain(AudioSourceComponent& audioSource, float gain);

    // Set pitch
    void setPitch(AudioSourceComponent& audioSource, float pitch);

    // Enable/disable looping
    void setLooping(AudioSourceComponent& audioSource, bool looping);

    // Set reference distance (start of attenuation)
    void setReferenceDistance(AudioSourceComponent& audioSource, float distance);

    // Set rolloff factor (attenuation curve)
    void setRolloffFactor(AudioSourceComponent& audioSource, float factor);

    // Set max distance (cutoff for attenuation)
    void setMaxDistance(AudioSourceComponent& audioSource, float maxDistance);

    // Set inner cone angle (area of normal sound)
    void setConeInnerAngle(AudioSourceComponent& audioSource, float angle);

    // Set outer cone angle (area where sound fades)
    void setConeOuterAngle(AudioSourceComponent& audioSource, float angle);

    // Set outer cone gain (volume reduction outside cone)
    void setConeOuterGain(AudioSourceComponent& audioSource, float gain);

    // Sets the audio clip of an audio source
    void setClip(AudioSourceComponent& audioSource, AudioClip* audioClip);

    // Syncs all settings of an audio source
    void sync(AudioSourceComponent& audioSource);

    // Plays the audio source
    void play(AudioSourceComponent& audioSource);

};