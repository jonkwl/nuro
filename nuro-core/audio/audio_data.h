#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <AL/al.h>
#include <type_traits>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#include <audio/audio_info.h>
#include <audio/audio_samples.h>

class AudioData 
{
public:
	AudioData();
	~AudioData();

	// Sets the path of the audio source file
	void setSource(const std::string& path);

	// Loads the audio data from the current source and sets audio info
	bool load(AudioInfo& info);

	// Frees any loaded audio data
	void free();

	// Returns the loaded pcm single channel samples (nullptr if not available)
	AudioSamples* monoSamples() const;

	// Returns the loaded pcm multichannel samples (nullptr if not available)
	AudioSamples* multichannelSamples() const;

	// Returns the current audio source file path
	std::string sourcePath() const;

private:
	// Decodes an audio stream into pcm samples, target layout and format provided by samples reference
	bool decodeInto(AudioSamples* samples, AVFormatContext*& formatContext, AVCodecContext*& codecContext, SwrContext*& swrContext, int streamIndex) const;

	// Returns the channel layout for the provided amount of channels
	AVChannelLayout getLayout_by_nChannels(int32_t nChannels) const;

	// Returns the channel layout for a provided channel layout mask
	AVChannelLayout getLayout_by_mask(uint64_t mask) const;

	// Handles process fail
	bool fail(std::string info) const;

	// Validates the current source path
	bool validateSource() const;

	std::string _sourcePath;
	AudioSamples* _monoSamples;
	AudioSamples* _multichannelSamples;
};