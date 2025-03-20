#include "audio_clip.h"

#include <utils/console.h>

bool AudioClip::loadIoData()
{
	return data.load();
}

void AudioClip::freeIoData()
{
	data.free();
}

bool AudioClip::uploadBuffers()
{
	return buffer.create(data);
}

void AudioClip::deleteBuffers()
{
	buffer.destroy();
}

AudioClip::AudioClip() : data(),
buffer()
{
}

AudioClip::~AudioClip()
{
	data.free();
	buffer.destroy();
}

void AudioClip::setSource(const std::string& path)
{
	data.setSource(path);
}

const AudioData& AudioClip::readData()
{
	return data;
}

const AudioBuffer& AudioClip::readBuffer()
{
	return buffer;
}