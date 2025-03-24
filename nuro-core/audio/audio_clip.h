#pragma once

#include <audio/audio_info.h>
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

	// Returns a readonly reference to the audio clips info
	const AudioInfo& info() const;

	// Returns a readonly reference to the audio clips data
	const AudioData& data() const;

	// Returns a readonly reference to the audio clips mono buffer
	const AudioBuffer& monoBuffer() const;

	// Returns a readonly reference to the audio clips multichannel buffer
	const AudioBuffer& multichannelBuffer() const;

	// Returns if the multichannel buffer is available
	bool multichannelAvailable() const;

	// Prints information about the audio clip
	void printInfo() const;

private:
	AudioInfo _info;
	AudioData _data;	

	AudioBuffer _monoBuffer;
	AudioBuffer _multichannelBuffer;

	bool _multichannelAvailable;
};