#pragma once

#include <string>
#include <cstdint>

struct AudioInfo {
	// Path to audio file
	std::string path = "NONE";

	//  Audio files name
	std::string name = "Unnamed";

	// File format
	std::string format = "INVALID";

	// Audio codec
	std::string codec = "NOT FOUND";

	// Samples per second
	int32_t sampleRate = 0;

	// Number of audio channels
	int32_t nChannels = 0;

	// Set if stereo is available
	bool stereo = false;

	// Bitrate in bit/s
	int32_t bitrate = 0;

	// Duration in seconds
	double duration = 0.0;

	// Metadata
	std::string metadata = "Empty";
};