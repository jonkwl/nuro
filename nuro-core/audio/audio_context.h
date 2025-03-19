#pragma once

#include <audio/audio_device.h>
#include <ecs/ecs_collection.h>

class ALCcontext;

class AudioContext 
{
public:
	AudioContext();

	// Creates the audio context
	void create();

	// Updates the audio context
	void update();

	// Handles any audio backend errors if any
	static void backendError();

public:
	// (ECS Bridge Callback) Constructs an audio source in backend
	static void constructAudioSource(Registry& reg, Entity ent);

	// (ECS Bridge Callback) Destroys an audio source in backend
	static void destroyAudioSource(Registry& reg, Entity ent);

private:
	ALCcontext* context;
	AudioDevice device;

};