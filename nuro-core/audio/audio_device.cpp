#include "audio_device.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <audio/audio_context.h>

AudioDevice::AudioDevice() : _handle(nullptr),
_opened(false),
_hardwareDevices(),
_usedHardware("System Default")
{
}

bool AudioDevice::create()
{
	_opened = false;
	_hardwareDevices.clear();

	// Open default device
	_handle = alcOpenDevice(nullptr);
	if (!_handle) {
		AudioContext::backendError();
		return false;
	}
	_opened = true;

	// Try to fetch hardware devices
	bool defaultDeviceOnly = true;
	if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT")) {
		const ALCchar* devices = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
		if (devices) {
			defaultDeviceOnly = false;
			while (*devices) {
				// Extract hardware device name
				std::string deviceName(devices);
				const std::string prefix = "OpenAL Soft on ";
				if (deviceName.rfind(prefix, 0) == 0) {
					deviceName = deviceName.substr(prefix.length());
				}

				// Add hardware device name
				_hardwareDevices.emplace_back(deviceName);
				devices += strlen(devices) + 1;
			}
		}
	}
	if (defaultDeviceOnly) _hardwareDevices.push_back("Default");

	return true;
}

void AudioDevice::destroy()
{
	if (!_handle) return;

	alcCloseDevice(_handle);
	_handle = nullptr;
	_opened = false;
}

bool AudioDevice::opened() const
{
	return _opened;
}

const std::vector<std::string>& AudioDevice::hardwareDevices() const
{
	return _hardwareDevices;
}

const std::string& AudioDevice::usedHardware() const {
	return _usedHardware;
}

ALCdevice* AudioDevice::handle()
{
	return _handle;
}