#pragma once

#include <string>
#include <vector>

class ALCdevice;

class AudioDevice 
{
public:
	AudioDevice();

	// Creates audio device
	bool create();

	// Closes the current audio device if any
	void close();

	// Refreshes audio device
	void refresh();

	// Returns if a device is opened
	bool opened() const;

	// Returns the currently fetched hardware devices
	const std::vector<std::string>& hardwareDevices() const;

	// Returns the name of the hardware device currently being used
	const std::string& usedHardware() const;

	// Return the current ausdio device handle (nullptr if none)
	ALCdevice* handle();

private:
	ALCdevice* _handle;
	bool _opened;
	std::vector<std::string> _hardwareDevices;
	std::string _usedHardware;
};