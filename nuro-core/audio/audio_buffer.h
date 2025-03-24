#pragma once

#include <vector>
#include <cstdint>
#include <AL/al.h>

#include <audio/audio_samples.h>

class AudioBuffer {
public:
	AudioBuffer();
	~AudioBuffer();

	// Uploads an audio buffer with the given audio samples
	bool create(AudioSamples& samples);

	// Deletes uploaded audio buffer if any
	void destroy();

	// Returns the audio buffers backend id
	uint32_t id() const;

	// Returns if the audio buffer is loaded
	bool loaded() const;

private:
	uint32_t _id;
	bool _loaded;
};