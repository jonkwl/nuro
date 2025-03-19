#include "audio_data.h"

#include <filesystem>

extern "C"
{
#include <libavcodec/avcodec.h>
}

#include <utils/console.h>

namespace fs = std::filesystem;

AudioData::AudioData() : path(),
avSamples(),
avLayout(AV_CHANNEL_LAYOUT_STEREO),
avFormat(AV_SAMPLE_FMT_S16),
avSampleRate(48000),
avChannels(2)
{
}

AudioData::~AudioData()
{
	free();
}

void AudioData::setSource(const std::string& _path)
{
	path = _path;
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
    AVPacket packet;
    av_init_packet(&packet);

    auto cleanup = [&]() {
        if (swrContext) swr_free(&swrContext);
        if (codecContext) avcodec_free_context(&codecContext);
        if (formatContext) avformat_close_input(&formatContext);
        };

    //
    // OPEN AUDIO FILE
    //

    if (avformat_open_input(&formatContext, path.c_str(), nullptr, nullptr) != 0) {
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

    avSampleRate = codecContext->sample_rate;

    //
    // DECODE AUDIO FRAMES
    //

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
                if (frame->format != avFormat) {
                    if (!swrContext) {
                        if (swr_alloc_set_opts2(&swrContext, &avLayout, AV_SAMPLE_FMT_S16, avSampleRate,
                            &codecContext->ch_layout, codecContext->sample_fmt,
                            codecContext->sample_rate, 0, nullptr) < 0) {
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
                    int outputBufferSize = av_samples_alloc(&outputBuffer, nullptr, avChannels, frame->nb_samples, AV_SAMPLE_FMT_S16, 0);
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

                    avSamples.insert(avSamples.end(), (int16_t*)outputBuffer, (int16_t*)outputBuffer + frame->nb_samples * avChannels);
                    av_freep(&outputBuffer);
                }
                else {
                    avSamples.insert(avSamples.end(), (int16_t*)frame->extended_data[0], (int16_t*)frame->extended_data[0] + frame->nb_samples * avChannels);
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
	avSamples.clear();
}

bool AudioData::loaded() const
{
	return !avSamples.empty();
}

const std::vector<int16_t>& AudioData::samples() const
{
	return avSamples;
}

size_t AudioData::size() const
{
	return avSamples.size() * sizeof(int16_t);
}

ALenum AudioData::format() const
{
	switch (avChannels) {
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
			return AL_FORMAT_STEREO8;
		case AV_SAMPLE_FMT_S16:
			return AL_FORMAT_STEREO16;
		default:
			return 0;
		}
	default:
		return 0;
	}
}

const uint32_t AudioData::sampleRate() const
{
	return avSampleRate;
}

std::string AudioData::sourcePath() const
{
	return path;
}

bool AudioData::fail(std::string info)
{
	Console::out::warning("Audio Data", info + " of audio data at '" + path + "'");
	return false;
}

bool AudioData::validateSource()
{
	if (!fs::exists(path)) 
		return fail("Could not find audio file");
	return true;
}
