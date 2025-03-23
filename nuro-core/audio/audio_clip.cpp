#include "audio_clip.h"

#include <utils/console.h>

bool AudioClip::loadIoData()
{
	return _data.load(_info);
}

void AudioClip::freeIoData()
{
	_data.free();
}

bool AudioClip::uploadBuffers()
{
    printInfo();

    if (!_monoBuffer.create(_data.monoSamples(), _info.sampleRate, AudioData::AL_MONO_FORMAT)) 
        return false;

    if (_info.stereo)
        if (_stereoBuffer.create(_data.stereoSamples(), _info.sampleRate, AudioData::AL_STEREO_FORMAT)) 
            _stereoAvailable = true;

    return true;
}

void AudioClip::deleteBuffers()
{
	_monoBuffer.destroy();
	_stereoBuffer.destroy();
}

AudioClip::AudioClip() : _info(),
_data(),
_stereoAvailable(false),
_monoBuffer(),
_stereoBuffer()
{
}

AudioClip::~AudioClip()
{
    freeIoData();
    deleteBuffers();
}

void AudioClip::setSource(const std::string& path)
{
	_data.setSource(path);
}

const AudioInfo& AudioClip::info() const
{
	return _info;
}

const AudioData& AudioClip::data() const
{
	return _data;
}

bool AudioClip::stereoAvailable() const
{
    return _stereoAvailable;
}

const AudioBuffer& AudioClip::monoBuffer() const
{
	return _monoBuffer;
}

const AudioBuffer& AudioClip::stereoBuffer() const
{
    return _stereoBuffer;
}

void AudioClip::printInfo() const
{
    Console::print
        >> Console::endl
        >> " nuro >>> Audio Clip \"" + resourceName() + "\":" >> Console::endl


        >> Console::endl
        >> " "
        >> Console::TextColor::MAGENTA
        >> Console::BgColor::WHITE
        >> " ---------- AUDIO INFO ---------- "
        >> Console::endl
        >> Console::endl
        >> Console::resetBg


        >> " Path: " >> _info.path >> Console::endl
        >> " Name: " >> _info.name >> Console::endl
        >> " Format: " >> _info.format >> Console::endl
        >> " Codec: " >> _info.codec >> Console::endl
        >> " Sample Rate: " >> std::to_string(_info.sampleRate) >> " Hz" >> Console::endl
        >> " Channels: " >> std::to_string(_info.nChannels) >> Console::endl
        >> " Stereo: " >> (_info.stereo ? "Yes" : "No") >> Console::endl
        >> " Bitrate: " >> std::to_string(_info.bitrate * 0.001) >> " kbit/s" >> Console::endl
        >> " Duration: " >> std::to_string(_info.duration) >> "s" >> Console::endl
        >> " Metadata: " >> (_info.metadata.empty() ? "None" : _info.metadata) >> Console::endl


        >> Console::endl
        >> " "
        >> Console::TextColor::MAGENTA
        >> Console::BgColor::WHITE
        >> " ---------- AUDIO INFO ---------- "
        >> Console::endl
        >> Console::endl
        >> Console::resetBg
        >> Console::resetText;
}