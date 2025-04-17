#include "audio_context.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <glm/gtc/type_ptr.hpp>

#include <utils/console.h>
#include <audio/audio_clip.h>
#include <audio/audio_source.h>
#include <transform/transform.h>
#include <context/application_context.h>

static LPALCGETSTRINGISOFT alcGetStringiSOFT;
static LPALCRESETDEVICESOFT alcResetDeviceSOFT;

template <typename T>
T loadProc(ALCdevice* _device, const char* funcName) {
	void* funcPtr = alcGetProcAddress(_device, funcName);
	return reinterpret_cast<T>(funcPtr);
}

AudioContext::AudioContext() : _context(nullptr),
_device(),
stereoAngles(false),
effects(false),
floatFmt(false),
multichannelFmt(false),
bFmt(false),
bHoaFmt(false),
hrtf(false),
hrtfList(),
activeHrtf("None")
{
}

void AudioContext::create()
{
	// Create device
	if (!_device.create()) {
		Console::out::warning("Audio Context", "Couldn't open default device");
		return;
	}
	ALCdevice* alDevice = _device.handle(); 

	// Create backend context
	_context = alcCreateContext(alDevice, NULL);
	if (!_context) {
		backendError();
		Console::out::warning("Audio Context", "Couldn't create backend context");
		return;
	}
	alcMakeContextCurrent(_context);

	// Check for extensions
	stereoAngles = alIsExtensionPresent("AL_EXT_STEREO_ANGLES");
	effects = alcIsExtensionPresent(alDevice, "ALC_EXT_EFX");

	floatFmt = alIsExtensionPresent("AL_EXT_float32");
	multichannelFmt = alIsExtensionPresent("AL_EXT_MCFORMATS");
	bFmt = alIsExtensionPresent("AL_EXT_BFORMAT");
	bHoaFmt = alIsExtensionPresent("AL_SOFT_bformat_hoa");

	// Setup hrtf
	setupHrtf();

	// Setup listener
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	ALfloat listenerOri[6] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
	alListenerfv(AL_ORIENTATION, listenerOri);
}

void AudioContext::destroy() {
	alcMakeContextCurrent(nullptr);
	_device.destroy();

	if (_context) {
		alcDestroyContext(_context);
		_context = nullptr;
	}
}

void AudioContext::update()
{
	if (!_context) return;

	// Sets if audio listener is currently being used
	bool listenerUsed = false;

	// Apply first audio listener
	auto audioListeners = ECS::main().view<TransformComponent, AudioListenerComponent>();
	for (auto [entity, transform, audioListener] : audioListeners.each()) {
		// Only use audio listener if its enabled
		if (!audioListener.enabled) break;
		 
		// Enable output
		alListenerf(AL_GAIN, 1.0f);

		// Update position
		glm::vec3 position = Transform::getPosition(transform, Space::WORLD);
		ALfloat positionData[3] = { position.x, position.y, position.z };
		alListenerfv(AL_POSITION, positionData);
		
		listenerUsed = true;
		break;
	}

	// Disable audio output if no listener is being used
	if (!listenerUsed) alListenerf(AL_GAIN, 0.0f);

	// Update audio sources
	auto audioSources = ECS::main().view<TransformComponent, AudioSourceComponent>();
	for (auto [entity, transform, audioSource] : audioSources.each()) {
		// Skip audio source if it isn't spatial
		if (!audioSource.isSpatial) continue;

		// Get audio sources backend id
		ALuint id = audioSource.id;

		// Update position
		glm::vec3 position = Transform::getPosition(transform, Space::WORLD);
		ALfloat positionData[3] = { position.x, position.y, position.z };
		alSourcefv(id, AL_POSITION, positionData);
	}
}

const AudioDevice& AudioContext::device() const {
	return _device;
}

bool AudioContext::stereoAnglesAvailable() const {
	return stereoAngles;
}

bool AudioContext::effectsAvailable() const
{
	return effects;
}

bool AudioContext::floatFmtAvailable() const
{
	return floatFmt;
}

bool AudioContext::multichannelFmtAvailable() const
{
	return multichannelFmt;
}

bool AudioContext::bFmtAvailable() const
{
	return bFmt;
}

bool AudioContext::bHoaFmtAvailable() const
{
	return bHoaFmt;
}

bool AudioContext::hrtfAvailable() const {
	return hrtf;
}

const std::string& AudioContext::usedHrtf() const {
	return activeHrtf;
}

const std::vector<std::string>& AudioContext::hrtfFunctions() const {
	return hrtfList;
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
	audioSource.id = static_cast<uint32_t>(sourceId);

	// Sync audio source
	AudioSource::sync(audioSource);
}

void AudioContext::destroyAudioSource(Registry& reg, Entity ent)
{
	// Get components
	TransformComponent& transform = reg.get<TransformComponent>(ent);
	AudioSourceComponent& audioSource = reg.get<AudioSourceComponent>(ent);

	// Destroy audio source in backend
	ALuint sourceId = static_cast<ALuint>(audioSource.id);
	alDeleteSources(1, &sourceId);
	audioSource.id = 0;
}

void AudioContext::setupHrtf() {
	// No hrtf available if function returns early
	hrtf = false;
	hrtfList.clear();
	activeHrtf = "None";

	// Check for hrtf extension
	ALCdevice* alDevice = _device.handle();
	if (!alcIsExtensionPresent(alDevice, "ALC_SOFT_HRTF")) return;

	// Load functions
	alcGetStringiSOFT = loadProc<LPALCGETSTRINGISOFT>(alDevice, "alcGetStringiSOFT");
	alcResetDeviceSOFT = loadProc<LPALCRESETDEVICESOFT>(alDevice, "alcResetDeviceSOFT");

	// Fetch all hrtfs
	ALCint nHrtf;
	alcGetIntegerv(alDevice, ALC_NUM_HRTF_SPECIFIERS_SOFT, 1, &nHrtf);
	for (int i = 0; i < nHrtf; i++)
	{
		const ALCchar* name = alcGetStringiSOFT(alDevice, ALC_HRTF_SPECIFIER_SOFT, i);
		hrtfList.push_back(std::string(name));

		// Enable first hrtf
		if (i == 0) {
			ALCint attributes[3] = { ALC_HRTF_SOFT, ALC_TRUE, 0 };
			if (!alcResetDeviceSOFT(alDevice, attributes)) {
				// Failed
				hrtfList.clear();
				return;
			}
		}
	}

	// Check if hrtf is being used
	ALCint hrtfUsed;
	alcGetIntegerv(alDevice, ALC_HRTF_SOFT, 1, &hrtfUsed);
	if (hrtfUsed) {
		const ALchar* name = alcGetString(alDevice, ALC_HRTF_SPECIFIER_SOFT);
		activeHrtf = std::string(name);
	}
	else {
		// Failed
		hrtfList.clear();
		return;
	}

	// Enabling hrtf succeeded
	hrtf = true;
}