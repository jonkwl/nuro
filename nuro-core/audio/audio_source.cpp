#include "audio_source.h"

#include <audio/audio_clip.h>

namespace AudioSource {

	void play(AudioSourceComponent& audioSource)
	{
		// Ensure audio source has audio clip
		if (!audioSource.clip) return;

		// Fetch audio clips buffer id
		uint32_t buffer = audioSource.clip->readBuffer().backendId();
		if (!buffer) return;

		// Associate buffer with audio source
		alSourcei(audioSource.backendId, AL_BUFFER, buffer);

		// Play audio source
		alSourcePlay(audioSource.backendId);
	}

};