
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

    // Create mono buffer (mandatory)
    AudioSamples* monoSamples = _data.monoSamples();
    if (!monoSamples) return false;
    if (!_monoBuffer.create(*monoSamples)) return false;

    // Create multichannel buffer if available
    AudioSamples* multichannelSamples = _data.multichannelSamples();
    if (multichannelSamples) {
        if (_multichannelBuffer.create(*multichannelSamples))
            _multichannelAvailable = true;
    }

    return true;
}

void AudioClip::deleteBuffers()
{
	_monoBuffer.destroy();
	_multichannelBuffer.destroy();
}

AudioClip::AudioClip() : _info(),
_data(),
_multichannelAvailable(false),
_monoBuffer(),
_multichannelBuffer()
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

bool AudioClip::multichannelAvailable() const
{
    return _multichannelAvailable;
}

const AudioBuffer& AudioClip::monoBuffer() const
{
	return _monoBuffer;
}

const AudioBuffer& AudioClip::multichannelBuffer() const
{
    return _multichannelBuffer;
}

void AudioClip::printInfo() const
{
    std::string metadata;
    if (_info.metadata.empty()) metadata = "None";
    else {
        for (const auto& [key, value] : _info.metadata)
            metadata.append("\n    - " + key + ": " + value);
    }

    Console::print
        >> Console::endl
        >> " nuro >>> Audio Clip \"" + resourceName() + "\":"


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
        >> " File Format: " >> _info.fileFormat >> Console::endl
        >> " Codec: " >> _info.codec >> Console::endl
        >> " Sample Rate: " >> std::to_string(_info.sampleRate) >> " Hz" >> Console::endl
        >> " Channels: " >> std::to_string(_info.nChannels) >> Console::endl
        >> " Sample Format: " >> _info.formatToStr(_info.sampleFormat) >> Console::endl
        >> " Bitrate: " >> std::to_string(_info.bitrate * 0.001) >> " kbit/s" >> Console::endl
        >> " Duration: " >> std::to_string(_info.duration) >> "s" >> Console::endl
        >> " Metadata: " >> Console::TextColor::GRAY >> metadata >> Console::endl


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