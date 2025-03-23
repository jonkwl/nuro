#include "audio_data.h"

#include <filesystem>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#include <utils/console.h>
#include <utils/ioutils.h>

namespace fs = std::filesystem;

AudioData::AudioData() : _info(),
_samples()
{
}

AudioData::~AudioData()
{
	free();
}

void AudioData::setSource(const std::string& path)
{
	_info.path = path;
	validateSource();
}

bool AudioData::load()
{
    if (!validateSource()) return false;

    //
    // RESOURCES
    //

    AVFormatContext* formatContext = nullptr;
    AVCodecContext* codecContext = nullptr;
    SwrContext* swrContext = nullptr;

    auto cleanup = [&]() {
        if (swrContext) swr_free(&swrContext);
        if (codecContext) avcodec_free_context(&codecContext);
        if (formatContext) avformat_close_input(&formatContext);
        };

    //
    // OPEN AUDIO FILE
    //

    if (avformat_open_input(&formatContext, _info.path.c_str(), nullptr, nullptr) != 0) {
        cleanup();
        return fail("Could not open audio file");
    }

    //
    // FIND AUDIO STREAM
    //

    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        cleanup();
        return fail("Could not find stream info");
    }

    const AVCodec* codec = nullptr;
    int streamIndex = -1;
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            codec = avcodec_find_decoder(formatContext->streams[i]->codecpar->codec_id);
            if (codec) {
                streamIndex = i;
                break;
            }
        }
    }

    if (streamIndex == -1) {
        cleanup();
        return fail("Could not find audio stream");
    }

    //
    // INITIALIZE DECODER
    //

    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext || avcodec_parameters_to_context(codecContext, formatContext->streams[streamIndex]->codecpar) < 0) {
        cleanup();
        return fail("Failed to allocate and set codec context");
    }

    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        cleanup();
        return fail("Failed to open codec");
    }

    //
    // FETCH AUDIO INFO
    //

    _info.name = IOUtils::getFilename(_info.path);
    _info.format = std::string(formatContext->iformat->name);
    _info.codec = codec->name;
    _info.sampleRate = codecContext->sample_rate;
    _info.nChannels = codecContext->ch_layout.nb_channels;
    _info.bitrate = formatContext->bit_rate;
    _info.duration = formatContext->duration / (double)AV_TIME_BASE;

    printInfo();

    //
    // DECODE AUDIO FRAMES
    //

    AVChannelLayout targetLayout = AV_CHANNEL_LAYOUT_MONO;
    AVSampleFormat targetFormat = AV_SAMPLE_FMT_S16;

    AVPacket packet;
    av_init_packet(&packet);

    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == streamIndex) {
            if (avcodec_send_packet(codecContext, &packet) < 0) {
                av_packet_unref(&packet);
                cleanup();
                return fail("Error sending packet for decoding");
            }

            AVFrame* frame = av_frame_alloc();
            if (!frame) {
                av_packet_unref(&packet);
                cleanup();
                return fail("Frame allocation failed");
            }

            while (avcodec_receive_frame(codecContext, frame) == 0) {
                if (frame->format != targetFormat) {
                    if (!swrContext) {
                        if (swr_alloc_set_opts2(&swrContext, &targetLayout, targetFormat, codecContext->sample_rate,&codecContext->ch_layout, codecContext->sample_fmt, codecContext->sample_rate, 0, nullptr) < 0)
                        {
                            av_frame_free(&frame);
                            av_packet_unref(&packet);
                            cleanup();
                            return fail("Failed to allocate and set SwrContext options");
                        }

                        if (swr_init(swrContext) < 0) {
                            av_frame_free(&frame);
                            av_packet_unref(&packet);
                            cleanup();
                            return fail("Failed to initialize resampler");
                        }
                    }

                    uint8_t* outputBuffer = nullptr;
                    int outputBufferSize = av_samples_alloc(&outputBuffer, nullptr, _info.nChannels, frame->nb_samples, targetFormat, 0);
                    if (outputBufferSize < 0) {
                        av_frame_free(&frame);
                        av_packet_unref(&packet);
                        cleanup();
                        return fail("Failed to allocate output buffer");
                    }

                    if (swr_convert(swrContext, &outputBuffer, frame->nb_samples, (const uint8_t**)frame->extended_data, frame->nb_samples) < 0) {
                        av_freep(&outputBuffer);
                        av_frame_free(&frame);
                        av_packet_unref(&packet);
                        cleanup();
                        return fail("Failed to convert audio samples");
                    }

                    _samples.insert(_samples.end(), (int16_t*)outputBuffer, (int16_t*)outputBuffer + frame->nb_samples * _info.nChannels);
                    av_freep(&outputBuffer);
                }
                else {
                    _samples.insert(_samples.end(), (int16_t*)frame->extended_data[0], (int16_t*)frame->extended_data[0] + frame->nb_samples * _info.nChannels);
                }
            }
            av_frame_free(&frame);
        }
        av_packet_unref(&packet);
    }

    cleanup();
    return true;
}

void AudioData::free()
{
    _samples.clear();
}

bool AudioData::loaded() const
{
	return !_samples.empty();
}

const AudioInfo& AudioData::info() const
{
    return _info;
}

const std::vector<int16_t>& AudioData::samples() const
{
	return _samples;
}

ALenum AudioData::format() const
{
	/*switch (_info.numChannels) {
	case 1: // Mono
		switch (avFormat) {
		case AV_SAMPLE_FMT_U8:
			return AL_FORMAT_MONO8;
		case AV_SAMPLE_FMT_S16:
			return AL_FORMAT_MONO16;
		default:
			return 0;
		}
	case 2: // Stereo
		switch (avFormat) {
		case AV_SAMPLE_FMT_U8:
			return AL_FORMAT_MONO8;
		case AV_SAMPLE_FMT_S16:
			return AL_FORMAT_MONO16;
		default:
			return 0;
		}
	default:
		return 0;
	}*/

    return AL_FORMAT_MONO16;
}

std::string AudioData::sourcePath() const
{
	return _info.path;
}

bool AudioData::fail(std::string info)
{
	Console::out::warning("Audio Data", info + " of audio data at '" + _info.path + "'");
	return false;
}

bool AudioData::validateSource()
{
	if (!fs::exists(_info.path))
		return fail("Could not find audio file");
	return true;
}

void AudioData::printInfo()
{
    Console::print
        >> Console::endl
        >> " nuro >>> Audio Data Reader [\"" + _info.name + "\"]:" >> Console::endl
        

        >> Console::endl
        >> " "
        >> Console::TextColor::MAGENTA
        >> Console::BgColor::WHITE
        >> " ---------- AUDIO FILE INFO ---------- "
        >> Console::endl
        >> Console::endl
        >> Console::resetBg


        >> " Name: " >> _info.name >> Console::endl
        >> " Format: " >> _info.format >> Console::endl
        >> " Codec: " >> _info.codec >> Console::endl
        >> " Sample Rate: " >> std::to_string(_info.sampleRate) >> " Hz" >> Console::endl
        >> " Channels: " >> std::to_string(_info.nChannels) >> Console::endl
        >> " Bitrate: " >> std::to_string(_info.bitrate * 0.001) >> " kbit/s" >> Console::endl
        >> " Duration: " >> std::to_string(_info.duration) >> "s" >> Console::endl


        >> Console::endl
        >> " "
        >> Console::TextColor::MAGENTA
        >> Console::BgColor::WHITE
        >> " ---------- AUDIO FILE INFO ---------- "
        >> Console::endl
        >> Console::endl
        >> Console::resetBg
        >> Console::resetText;
}