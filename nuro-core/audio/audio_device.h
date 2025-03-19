#pragma once

#include <string>

class ALCdevice;

class AudioDevice 
{
public:
	AudioDevice();

	// Opens an audio device by its name
	bool open(const char* name);

	// Opens the first available audio device
	bool open();

	// Closes the current audio device if any
	void close();

	// Returns if a device is opened
	bool opened() const;

	// Returns the name of the current audio device
	std::string name() const;

	// Returns the current audio device handle (nullptr if none)
	ALCdevice* handle();

private:
	// Fetches and caches the name of the current audio device
	void fetchName();

	ALCdevice* backendHandle;

	std::string cachedName;
};