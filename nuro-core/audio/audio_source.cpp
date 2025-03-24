#include "audio_source.h"

#include <audio/audio_clip.h>

namespace AudioSource {

    // Binds the buffer of the audio sources clip to its underlying backend source
    bool _bindBuffer(AudioSourceComponent& audioSource) {
        // Ensure audio source has clip
        if (!audioSource.clip) return false;

        // Stop source playback
        stop(audioSource);

        // Fetch audio buffer from clip
        bool forceMono = audioSource.isSpatial;
        bool useMultichannel = audioSource.clip->multichannelAvailable() && !forceMono;
        const AudioBuffer& buffer = useMultichannel ? audioSource.clip->multichannelBuffer() : audioSource.clip->monoBuffer();

        // Bind buffer to source
        if (!buffer.id()) return false;
        alSourcei(audioSource.id, AL_BUFFER, buffer.id());

        // Update using multichannel
        audioSource.usingMultichannel = useMultichannel;

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
        // Fetch initial source state
        State state = getState(audioSource);
        float offset = getOffset(audioSource);

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

        // Restore previous play state
        if (state == State::PLAYING) play(audioSource, offset);
    }

    void setRange(AudioSourceComponent& audioSource, float range) {
        alSourcef(audioSource.id, AL_MAX_DISTANCE, range);
        audioSource.range = range;
    }

    void setFalloff(AudioSourceComponent& audioSource, float falloff) {
        alSourcef(audioSource.id, AL_ROLLOFF_FACTOR, falloff);
        audioSource.falloff = falloff;
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

    void setClip(AudioSourceComponent& audioSource, ResourceRef<AudioClip> clip)
    {
        audioSource.clip = clip;
    }

    void sync(AudioSourceComponent& audioSource)
    {
        // Individual properties
        setVolume(audioSource, audioSource.volume);
        setPitch(audioSource, audioSource.pitch);
        setLooping(audioSource, audioSource.looping);
        setRange(audioSource, audioSource.range);
        setFalloff(audioSource, audioSource.falloff);
        setConeInnerAngle(audioSource, audioSource.coneInnerAngle);
        setConeOuterAngle(audioSource, audioSource.coneOuterAngle);
        setConeOuterVolume(audioSource, audioSource.coneOuterVolume);

        // Combined properties
        setIsSpatial(audioSource, audioSource.isSpatial);
    }

    State getState(AudioSourceComponent& audioSource)
    {
        ALint state;
        alGetSourcei(audioSource.id, AL_SOURCE_STATE, &state);
        switch (state) {
        case AL_PLAYING:
            return State::PLAYING;
        case AL_PAUSED:
            return State::PAUSED;
        default:
            return State::STOPPED;
        }
    }

    void play(AudioSourceComponent& audioSource, float offset)
    {
        // Always bind buffer from current audio clip
        if (!_bindBuffer(audioSource)) return;

        // Set playback offset if given
        if (offset) setOffset(audioSource, offset);

        alSourcePlay(audioSource.id);
    }

    void pause(AudioSourceComponent& audioSource)
    {
        alSourcePause(audioSource.id);
    }

    void stop(AudioSourceComponent& audioSource)
    {
        alSourceStop(audioSource.id);
    }

    void resume(AudioSourceComponent& audioSource)
    {
        play(audioSource, getOffset(audioSource));
    }

    float getOffset(AudioSourceComponent& audioSource)
    {
        float offset;
        alGetSourcef(audioSource.id, AL_SEC_OFFSET, &offset);
        return offset;
    }

    void setOffset(AudioSourceComponent& audioSource, float offset)
    {
        alSourcef(audioSource.id, AL_SEC_OFFSET, offset);
    }

};