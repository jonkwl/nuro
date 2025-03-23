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

template <typename T>
struct AudioSamples {
	static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "T of AudioSamples must be a numeric type");

	// Raw pcm audio samples
	std::vector<T> data;

	// Format of pcm audio samples in backend
	ALenum format;
};

class AudioData 
{
public:
	AudioData();
	~AudioData();

	// Standard audio stream target format (16-bit signed integer pcm format)
	static constexpr AVSampleFormat AV_TARGET_FORMAT = AV_SAMPLE_FMT_S16;

	// Audio backend format of mono samples
	static constexpr ALenum AL_MONO_FORMAT = AL_FORMAT_MONO16;

	// Audio backend format of stereo samples
	static constexpr ALenum AL_STEREO_FORMAT = AL_FORMAT_STEREO16;

	// Sets the path of the audio source file
	void setSource(const std::string& path);

	// Loads the audio data from the current source and sets audio info
	bool load(AudioInfo& info);

	// Frees any loaded audio data
	void free();

	// Returns the loaded pcm mono samples
	const std::vector<int16_t>& monoSamples() const;

	// Returns the loaded pcm stereo samples if available
	const std::vector<int16_t>& stereoSamples() const;

	// Returns the current audio source file path
	std::string sourcePath() const;

private:
	// Decodes an existing audio stream into pcm samples, converting its format and layout if needed
	bool AudioData::decodeInto(std::vector<int16_t>& samples, AVFormatContext*& formatContext, AVCodecContext*& codecContext, SwrContext*& swrContext, int streamIndex, AVChannelLayout targetLayout, AVSampleFormat targetFormat) const;

	// Handles process fail
	bool fail(std::string info) const;

	// Validates the current source path
	bool validateSource() const;

	std::string _sourcePath;
	std::vector<int16_t> _monoSamples;
	std::vector<int16_t> _stereoSamples;
};