#pragma once

#include <vector>
#include <cstdint>
#include <AL/al.h>

#include <audio/audio_data.h>
#include <audio/audio_info.h>

class AudioBuffer {
public:
	AudioBuffer();
	~AudioBuffer();

	// Uploads an audio buffer with the given audio data
	bool create(const std::vector<int16_t>& pcmSamples, int32_t sampleRate, ALenum format);

	// Deletes uploaded audio buffer if any
	void destroy();

	// Returns the audio buffers backend id
	uint32_t id() const;

private:
	uint32_t _id;
};