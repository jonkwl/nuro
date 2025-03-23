#include "audio_buffer.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <utils/console.h>

AudioBuffer::AudioBuffer() : _id(0)
{
}

AudioBuffer::~AudioBuffer()
{
	destroy();
}

bool AudioBuffer::create(const AudioData& data)
{
	if (!data.loaded()) {
		Console::out::warning("Audio Buffer", "Tried to create audio buffer with empty data");
		return false;
	}

	ALuint bufferId;
	alGenBuffers(1, &bufferId);
	if (alGetError() != AL_NO_ERROR) {
		Console::out::warning("Audio Buffer", "Couldn't create audio buffer");
		return false;
	}

	const AudioInfo& info = data.info();
	const std::vector<int16_t>& samples = data.samples();
	size_t size = samples.size() * sizeof(int16_t);

	alBufferData(bufferId, data.format(), samples.data(), size, info.sampleRate);
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
