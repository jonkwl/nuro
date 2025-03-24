#pragma once

#include <ecs/components.h>

class AudioClip;

namespace AudioSource {

    enum State {
        PLAYING,
        PAUSED,
        STOPPED
    };

    // Set gain (volume)
    void setVolume(AudioSourceComponent& audioSource, float volume);

    // Set pitch
    void setPitch(AudioSourceComponent& audioSource, float pitch);

    // Enables or disables looping
    void setLooping(AudioSourceComponent& audioSource, bool looping);

    // Makes the source use 3D spatial audio or not
    void setIsSpatial(AudioSourceComponent& audioSource, bool spatial);

    // Set max distance (cutoff for attenuation)
    void setRange(AudioSourceComponent& audioSource, float range);

    // Set rolloff factor (attenuation curve)
    void setFalloff(AudioSourceComponent& audioSource, float falloff);

    // Set inner cone angle (area of normal sound)
    void setConeInnerAngle(AudioSourceComponent& audioSource, float angle);

    // Set outer cone angle (area where sound fades)
    void setConeOuterAngle(AudioSourceComponent& audioSource, float angle);

    // Set outer cone gain (volume reduction outside cone)
    void setConeOuterVolume(AudioSourceComponent& audioSource, float volume);

    // Sets the clip of an audio source
    void setClip(AudioSourceComponent& audioSource, AudioClip* clip);

    // Syncs all settings of an audio source
    void sync(AudioSourceComponent& audioSource);

    // Returns the state of an audio source
    State getState(AudioSourceComponent& audioSource);

    // Starts the playback
    void play(AudioSourceComponent& audioSource, float offset = 0.0f);

    // Pauses the playback
    void pause(AudioSourceComponent& audioSource);

    // Stops the playback
    void stop(AudioSourceComponent& audioSource);

    // Continues the playback where it last stopped
    void resume(AudioSourceComponent& audioSource);

    // Returns the current playback offset
    float getOffset(AudioSourceComponent& audioSource);

    // Sets the current playback offset
    void setOffset(AudioSourceComponent& audioSource, float offset);

};