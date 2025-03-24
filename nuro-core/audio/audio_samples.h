#pragma once

#include <al/al.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

class AudioSamples {
public:
	virtual ~AudioSamples() = default;

	virtual int32_t getSampleRate() const = 0;
	virtual AVChannelLayout getAvTargetLayout() const = 0;
	virtual AVSampleFormat getAvTargetFormat() const = 0;
	virtual ALenum getAlFormat() const = 0;

	virtual void* getSamples() = 0;
	virtual size_t getSize() const = 0;
	virtual void insertSamples(const void* start, const void* end) = 0;
};

template <typename T>
class AudioSamplesBuffer : public AudioSamples {
public:
	AudioSamplesBuffer(int32_t sampleRate, AVChannelLayout avTargetLayout, AVSampleFormat avTargetFormat, ALenum alFormat)
		: sampleRate(sampleRate), avTargetLayout(avTargetLayout), avTargetFormat(avTargetFormat), alFormat(alFormat)
	{
	}

	// Returns the sample rate of the samples
	int32_t getSampleRate() const override {
		return sampleRate;
	}

	// Returns the ffmpeg target channel layout when decoding the audio samples
	AVChannelLayout getAvTargetLayout() const override {
		return avTargetLayout;
	}

	// Returns the ffmpeg target format when decoding the audio samples
	AVSampleFormat getAvTargetFormat() const override {
		return avTargetFormat;
	}

	// Returns the audio backend format when buffer the audio samples
	ALenum getAlFormat() const override {
		return alFormat;
	}

	// Returns a pointer to the raw pcm samples
	void* getSamples() override {
		return samples.data();
	}

	// Returns the size of the samples
	size_t getSize() const override {
		return samples.size() * sizeof(T);
	}

	// Inserts a range of samples
	void insertSamples(const void* start, const void* end) override {
		const T* typedStart = static_cast<const T*>(start);
		const T* typedEnd = static_cast<const T*>(end);
		samples.insert(samples.end(), typedStart, typedEnd);
	}

private:
	const int32_t sampleRate;
	const AVChannelLayout avTargetLayout;
	const AVSampleFormat avTargetFormat;
	const ALenum alFormat;

	std::vector<T> samples;
};