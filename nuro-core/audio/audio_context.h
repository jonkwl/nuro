#pragma once

#include <vector>
#include <string>

#include <audio/audio_device.h>
#include <ecs/ecs_collection.h>

class ALCcontext;

class AudioContext 
{
public:
	AudioContext();

	// Creates audio context
	void create();

	// Closes the current audio context if any
	void close();

	// Refreshes audio context
	void refresh();

	// Updates the audio context
	void update();

	// Returns the audio device
	const AudioDevice& device() const;

	// Returns if stereo angles are available
	bool stereoAnglesAvailable() const;

	// Returns if hrtf is available
	bool hrtfAvailable() const;

	// Returns name of hrtf function being used
	const std::string& usedHrtf() const;

	// Returns the names of all hrtf functions available
	const std::vector<std::string>& hrtfFunctions() const;

	// Handles any audio backend errors if any
	static void backendError();

public:
	// (ECS Bridge Callback) Constructs an audio source in backend
	static void constructAudioSource(Registry& reg, Entity ent);

	// (ECS Bridge Callback) Destroys an audio source in backend
	static void destroyAudioSource(Registry& reg, Entity ent);

private:
	void setupHrtf();

	ALCcontext* _context;
	AudioDevice _device;

	bool stereoAngles;
	bool effects;
	bool hrtf;
	std::vector<std::string> hrtfList;
	std::string activeHrtf;
};