#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <AL/al.h>

// Holds information about the audio source
struct AudioInfo {
	// Path to audio file
	std::string path;

	//  Audio files name
	std::string name;

	// File format
	std::string format;

	// Audio codec
	std::string codec;

	// Samples per second
	int32_t sampleRate = 0;

	// Number of audio channels
	int32_t nChannels = 0;

	// Bitrate in bit/s
	int32_t bitrate = 0;

	// Duration in seconds
	double duration = 0.0;

	// Metadata
	std::string metadata;
};

class AudioData 
{
public:
	AudioData();
	~AudioData();

	// Sets the path of the audio source file
	void setSource(const std::string& path);

	// Loads the audio data from the current source
	bool load();

	// Frees any loaded audio data
	void free();

	// Returns if there is any audio data loaded
	bool loaded() const;

	// Returns the audio info
	const AudioInfo& info() const;

	// Returns the loaded samples
	const std::vector<int16_t>& samples() const;

	// Returns the format of the audio data
	ALenum format() const;

	// Returns the current audio source file path
	std::string sourcePath() const;

private:
	// Handles process fail
	bool fail(std::string info);

	// Validates the current source path
	bool validateSource();

	// Prints the current audio info
	void printInfo();

	AudioInfo _info;
	std::vector<int16_t> _samples;
};