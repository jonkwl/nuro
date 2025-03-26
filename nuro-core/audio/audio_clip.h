#pragma once

#include <memory/resource.h>
#include <audio/audio_info.h>
#include <audio/audio_data.h>
#include <audio/audio_buffer.h>

class AudioClip : public Resource
{
public:
	AudioClip();
	~AudioClip() override;

	// Default pipe for creating audio clip
	ResourcePipe create() {
		return std::move(pipe()
			>> BIND_TASK(AudioClip, loadIoData)
			>> BIND_TASK_WITH_FLAGS(AudioClip, uploadBuffers, TaskFlags::UseContextThread));
	}

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
	bool loadIoData();
	void freeIoData();
	bool uploadBuffers();
	void deleteBuffers();

	AudioInfo _info;
	AudioData _data;	

	AudioBuffer _monoBuffer;
	AudioBuffer _multichannelBuffer;

	bool _multichannelAvailable;
};