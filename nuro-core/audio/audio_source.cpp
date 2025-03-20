#include "audio_source.h"

#include <audio/audio_clip.h>

namespace AudioSource {

    void setGain(AudioSourceComponent& audioSource, float gain) {
        alSourcef(audioSource.backendId, AL_GAIN, gain);
        audioSource.gain = gain;
    }

    void setPitch(AudioSourceComponent& audioSource, float pitch) {
        alSourcef(audioSource.backendId, AL_PITCH, pitch);
        audioSource.pitch = pitch;
    }

    void setLooping(AudioSourceComponent& audioSource, bool looping) {
        alSourcei(audioSource.backendId, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
        audioSource.looping = looping;
    }

    void setReferenceDistance(AudioSourceComponent& audioSource, float distance) {
        alSourcef(audioSource.backendId, AL_REFERENCE_DISTANCE, distance);
        audioSource.referenceDistance = distance;
    }

    void setRolloffFactor(AudioSourceComponent& audioSource, float factor) {
        alSourcef(audioSource.backendId, AL_ROLLOFF_FACTOR, factor);
        audioSource.rolloffFactor = factor;
    }

    void setMaxDistance(AudioSourceComponent& audioSource, float maxDistance) {
        alSourcef(audioSource.backendId, AL_MAX_DISTANCE, maxDistance);
        audioSource.maxDistance = maxDistance;
    }

    void setConeInnerAngle(AudioSourceComponent& audioSource, float angle) {
        alSourcef(audioSource.backendId, AL_CONE_INNER_ANGLE, angle);
        audioSource.coneInnerAngle = angle;
    }

    void setConeOuterAngle(AudioSourceComponent& audioSource, float angle) {
        alSourcef(audioSource.backendId, AL_CONE_OUTER_ANGLE, angle);
        audioSource.coneOuterAngle = angle;
    }

    void setConeOuterGain(AudioSourceComponent& audioSource, float gain) {
        alSourcef(audioSource.backendId, AL_CONE_OUTER_GAIN, gain);
        audioSource.coneOuterGain = gain;
    }

    void setClip(AudioSourceComponent& audioSource, AudioClip* audioClip)
    {
        audioSource.clip = audioClip;
    }

    void sync(AudioSourceComponent& audioSource)
    {
        setGain(audioSource, audioSource.gain);
        setPitch(audioSource, audioSource.pitch);
        setLooping(audioSource, audioSource.looping);
        setReferenceDistance(audioSource, audioSource.referenceDistance);
        setRolloffFactor(audioSource, audioSource.rolloffFactor);
        setMaxDistance(audioSource, audioSource.maxDistance);
        setConeInnerAngle(audioSource, audioSource.coneInnerAngle);
        setConeOuterAngle(audioSource, audioSource.coneOuterAngle);
        setConeOuterGain(audioSource, audioSource.coneOuterGain);
    }

    void play(AudioSourceComponent& audioSource)
    {
        if (!audioSource.clip) return;

        // Associate audio source with buffer
        uint32_t buffer = audioSource.clip->readBuffer().backendId();
        if (!buffer) return;
        alSourcei(audioSource.backendId, AL_BUFFER, buffer);

        // Play audio clip
        alSourcePlay(audioSource.backendId);
    }

};