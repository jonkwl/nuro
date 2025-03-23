#pragma once

#include <cstdint>

#include <audio/audio_data.h>

class AudioBuffer {
public:
	AudioBuffer();
	~AudioBuffer();

	// Uploads an audio buffer with the given audio data
	bool create(const AudioData& data);

	// Deletes uploaded audio buffer if any
	void destroy();

	// Returns the audio buffers backend id
	uint32_t id() const;

private:
	uint32_t _id;
};