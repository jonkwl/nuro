#pragma once

#include <audio/audio_data.h>
#include <resource/resource.h>
#include <audio/audio_buffer.h>

class AudioClip : public Resource
{
protected:
	bool loadIoData() override;
	void freeIoData() override;
	bool uploadBuffers() override;
	void deleteBuffers() override;

public:
	AudioClip();
	~AudioClip();

	// Sets the path of the audio clips data source
	void setSource(const std::string& path);

	// Returns a readonly reference to the audio clips data
	const AudioData& readData();

	// Returns a readonly reference to the audio clips buffer
	const AudioBuffer& readBuffer();

	std::string sourcePath() override;

private:
	AudioData data;
	AudioBuffer buffer;
};