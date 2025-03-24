#include "audio_buffer.h"

#include <AL/alc.h>

#include <utils/console.h>

AudioBuffer::AudioBuffer() : _id(0),
_loaded(false)
{
}

AudioBuffer::~AudioBuffer()
{
	destroy();
}

bool AudioBuffer::create(AudioSamples& samples)
{
	ALuint bufferId;
	alGenBuffers(1, &bufferId);
	if (alGetError() != AL_NO_ERROR) {
		Console::out::warning("Audio Buffer", "Couldn't create audio buffer");
		return false;
	}

	alBufferData(bufferId, samples.getAlFormat(), samples.getSamples(), samples.getSize(), samples.getSampleRate());
	if (alGetError() != AL_NO_ERROR) {
		Console::out::warning("Audio Buffer", "Couldn't load audio data into buffer");
		return false;
	}

	_id = static_cast<uint32_t>(bufferId);
	_loaded = true;

	return true;
}

void AudioBuffer::destroy()
{
	if (_id) alDeleteBuffers(1, &_id);
	_id = 0;
	_loaded = false;
}

uint32_t AudioBuffer::id() const
{
	return _id;
}

bool AudioBuffer::loaded() const
{
	return _loaded;
}
