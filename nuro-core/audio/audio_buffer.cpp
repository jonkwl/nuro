#include "audio_buffer.h"

#include <AL/alc.h>

#include <utils/console.h>

AudioBuffer::AudioBuffer() : _id(0)
{
}

AudioBuffer::~AudioBuffer()
{
	destroy();
}

bool AudioBuffer::create(const std::vector<int16_t>& pcmSamples, int32_t sampleRate, ALenum format)
{
	ALuint bufferId;
	alGenBuffers(1, &bufferId);
	if (alGetError() != AL_NO_ERROR) {
		Console::out::warning("Audio Buffer", "Couldn't create audio buffer");
		return false;
	}

	size_t size = pcmSamples.size() * sizeof(int16_t);

	alBufferData(bufferId, format, pcmSamples.data(), size, sampleRate);
	if (alGetError() != AL_NO_ERROR) {
		Console::out::warning("Audio Buffer", "Couldn't load audio data into buffer");
		return false;
	}

	_id = static_cast<uint32_t>(bufferId);
	return true;
}

void AudioBuffer::destroy()
{
	if (_id) alDeleteBuffers(1, &_id);
	_id = 0;
}

uint32_t AudioBuffer::id() const
{
	return _id;
}
