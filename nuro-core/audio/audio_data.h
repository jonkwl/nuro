#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <AL/al.h>

extern "C"
{
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

class AudioData 
{
public:
	AudioData();
	~AudioData();

	// Sets the path of the audios source
	void setSource(const std::string& path);

	// Loads the audio data from the current source
	bool load();

	// Frees any loaded audio data
	void free();

	// Returns if there is any audio data loaded
	bool loaded() const;

	// Returns the loaded samples
	const std::vector<int16_t>& samples() const;

	// Returns the size of the loaded samples
	size_t size() const;

	// Returns the format of the audio data
	ALenum format() const;

	// Returns the sample rate of the audio data
	const uint32_t sampleRate() const;

	// Returns the current audio data source path
	std::string sourcePath() const;

private:
	// Handles process fail
	bool fail(std::string info);

	// Validates the current source path
	bool validateSource();

	// Path to the audio data source
	std::string path;

	// Layout of audio data
	AVChannelLayout avLayout;

	// Format of audio data
	AVSampleFormat avFormat;

	// Sample rate of audio
	uint32_t avSampleRate;

	// Amount of channels of audio
	uint32_t avChannels;

	// Raw audio data storage
	std::vector<int16_t> avSamples;
};