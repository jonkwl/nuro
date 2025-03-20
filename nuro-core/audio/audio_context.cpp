#include "audio_context.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <glm/gtc/type_ptr.hpp>

#include <utils/console.h>
#include <audio/audio_clip.h>
#include <audio/audio_source.h>
#include <transform/transform.h>
#include <context/application_context.h>

AudioContext::AudioContext() : context(nullptr),
device()
{
}

void AudioContext::create()
{
	// Open device
	if (!device.open()) {
		Console::out::warning("Audio Context", "Couldn't open default device");
		return;
	}

	// Create backend context
	context = alcCreateContext(device.handle(), NULL);
	if (!context) {
		backendError();
		Console::out::warning("Audio Context", "Couldn't create backend context");
		return;
	}
	alcMakeContextCurrent(context);
}

void AudioContext::update()
{
	// Tmp testing code

	// Sets if audio listener is currently being used
	bool listenerUsed = false;

	// Fetch and sync first audio listener
	auto audioListeners = ECS::main().view<TransformComponent, AudioListenerComponent>();
	for (auto& [entity, transform, audioListener] : audioListeners.each()) {
		if (!audioListener.enabled) break;
		listenerUsed = true;

		// Update gain
		alListenerf(AL_GAIN, audioListener.gain);

		// Update position
		glm::vec3 position = Transform::getPosition(transform, Space::WORLD);
		ALfloat positionData[3] = { position.x, position.y, position.z };
		alListenerfv(AL_POSITION, positionData);

		break;
	}

	// Disable audio output if no listener is being used
	if (!listenerUsed) alListenerf(AL_GAIN, 0.0f);
}

void AudioContext::backendError()
{
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) 
		Console::out::warning("Audio Context", "Backend Error: " + std::string(alGetString(error)));
}

void AudioContext::constructAudioSource(Registry& reg, Entity ent)
{
	// Get components
	TransformComponent& transform = reg.get<TransformComponent>(ent);
	AudioSourceComponent& audioSource = reg.get<AudioSourceComponent>(ent);

	// Create audio source in backend
	ALuint sourceId;
	alGenSources(1, &sourceId);

	// Link audio source to component
	audioSource.backendId = static_cast<uint32_t>(sourceId);

	// Sync audio source
	AudioSource::sync(audioSource);
}

void AudioContext::destroyAudioSource(Registry& reg, Entity ent)
{
	// Get components
	TransformComponent& transform = reg.get<TransformComponent>(ent);
	AudioSourceComponent& audioSource = reg.get<AudioSourceComponent>(ent);

	// Destroy audio source in backend
	ALuint sourceId = static_cast<ALuint>(audioSource.backendId);
	alDeleteSources(1, &sourceId);
	audioSource.backendId = 0;
}
