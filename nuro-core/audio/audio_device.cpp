#include "audio_device.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <audio/audio_context.h>

AudioDevice::AudioDevice() : backendHandle(nullptr),
cachedName()
{
}

bool AudioDevice::open(const char* name)
{
	ALCdevice* _tmpHandle = alcOpenDevice(name);

	if (!_tmpHandle) {
		AudioContext::backendError();
		return false;
	}

	backendHandle = _tmpHandle;
	fetchName();

	return true;
}

bool AudioDevice::open()
{
	return open(nullptr);
}

void AudioDevice::close()
{
	if (backendHandle) alcCloseDevice(backendHandle);
}

bool AudioDevice::opened() const
{
	return backendHandle;
}

std::string AudioDevice::name() const
{
	return cachedName;
}

ALCdevice* AudioDevice::handle()
{
	return backendHandle;
}

void AudioDevice::fetchName()
{
	if (!backendHandle) {
		cachedName = "NONE";
		return;
	}

	const ALchar* name = alcGetString(backendHandle, ALC_DEVICE_SPECIFIER);
	if (!name) {
		cachedName = "NONE";
		return;
	}

	cachedName = std::string(name);
}