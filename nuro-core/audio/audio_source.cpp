#include "audio_source.h"

#include <audio/audio_clip.h>

namespace AudioSource {

    // Binds the buffer of the audio sources clip to its underlying backend source
    bool _bindBuffer(AudioSourceComponent& audioSource) {
        // Ensure audio source has clip
        if (!audioSource.clip) return false;

        // Stop source playback
        stop(audioSource);

        // Force mono for spatial playback
        bool forceMono = audioSource.isSpatial;

        // Select buffer and bind it to sourcé
        uint32_t buffer = audioSource.clip->readBuffer().id();
        if (!buffer) return false;
        alSourcei(audioSource.id, AL_BUFFER, buffer);

        return true;
    }

    void setVolume(AudioSourceComponent& audioSource, float volume) {
        alSourcef(audioSource.id, AL_GAIN, volume);
        audioSource.volume = volume;
    }

    void setPitch(AudioSourceComponent& audioSource, float pitch) {
        alSourcef(audioSource.id, AL_PITCH, pitch);
        audioSource.pitch = pitch;
    }

    void setLooping(AudioSourceComponent& audioSource, bool looping) {
        alSourcei(audioSource.id, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
        audioSource.looping = looping;
    }

    void setIsSpatial(AudioSourceComponent& audioSource, bool spatial)
    {
        if (spatial) {
            // Make source use absolute position
            alSourcei(audioSource.id, AL_SOURCE_RELATIVE, AL_FALSE);

            // Set sources spatial properties
            setFalloff(audioSource, audioSource.falloff);
            setConeInnerAngle(audioSource, audioSource.coneInnerAngle);
            setConeOuterAngle(audioSource, audioSource.coneOuterAngle);
        }
        else {
            // Make source relative
            alSourcei(audioSource.id, AL_SOURCE_RELATIVE, AL_TRUE);

            // Reset sources position and velocity
            alSource3f(audioSource.id, AL_POSITION, 0.0f, 0.0f, 0.0f);
            alSource3f(audioSource.id, AL_VELOCITY, 0.0f, 0.0f, 0.0f);

            // Disable distance attenuation and make source fully omnidirectional
            setFalloff(audioSource, 0.0f);
            setConeInnerAngle(audioSource, 360.0f);
            setConeOuterAngle(audioSource, 360.0f);
        }

        // Rebind buffer
        _bindBuffer(audioSource);
    }

    void setFalloff(AudioSourceComponent& audioSource, float falloff) {
        alSourcef(audioSource.id, AL_ROLLOFF_FACTOR, falloff);
        audioSource.falloff = falloff;
    }

    void setRange(AudioSourceComponent& audioSource, float range) {
        alSourcef(audioSource.id, AL_MAX_DISTANCE, range);
        audioSource.range = range;
    }

    void setConeInnerAngle(AudioSourceComponent& audioSource, float angle) {
        alSourcef(audioSource.id, AL_CONE_INNER_ANGLE, angle);
        audioSource.coneInnerAngle = angle;
    }

    void setConeOuterAngle(AudioSourceComponent& audioSource, float angle) {
        alSourcef(audioSource.id, AL_CONE_OUTER_ANGLE, angle);
        audioSource.coneOuterAngle = angle;
    }

    void setConeOuterVolume(AudioSourceComponent& audioSource, float volume) {
        alSourcef(audioSource.id, AL_CONE_OUTER_GAIN, volume);
        audioSource.coneOuterVolume = volume;
    }

    void sync(AudioSourceComponent& audioSource)
    {
        // Individual properties
        setVolume(audioSource, audioSource.volume);
        setPitch(audioSource, audioSource.pitch);
        setLooping(audioSource, audioSource.looping);
        setFalloff(audioSource, audioSource.falloff);
        setRange(audioSource, audioSource.range);
        setConeInnerAngle(audioSource, audioSource.coneInnerAngle);
        setConeOuterAngle(audioSource, audioSource.coneOuterAngle);
        setConeOuterVolume(audioSource, audioSource.coneOuterVolume);

        // Combined properties
        setIsSpatial(audioSource, audioSource.isSpatial);
    }

    void play(AudioSourceComponent& audioSource)
    {
        if (!_bindBuffer(audioSource)) return;
        alSourcePlay(audioSource.id);
    }

    void stop(AudioSourceComponent& audioSource)
    {
        alSourceStop(audioSource.id);
    }

};