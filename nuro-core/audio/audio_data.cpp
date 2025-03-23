#include "audio_data.h"

#include <filesystem>

#include <utils/console.h>
#include <utils/ioutils.h>

namespace fs = std::filesystem;

AudioData::AudioData() : _sourcePath(),
_monoSamples(),
_stereoSamples()
{
}

AudioData::~AudioData()
{
	free();
}

void AudioData::setSource(const std::string& path)
{
    _sourcePath = path;
	validateSource();
}

bool AudioData::load(AudioInfo& info)
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

    if (avformat_open_input(&formatContext, _sourcePath.c_str(), nullptr, nullptr) != 0) {
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

    info.path = _sourcePath;
    info.name = IOUtils::getFilename(info.path);
    info.format = std::string(formatContext->iformat->name);
    info.codec = codec->name;
    info.sampleRate = codecContext->sample_rate;
    info.nChannels = codecContext->ch_layout.nb_channels;
    info.stereo = info.nChannels == 2;
    info.bitrate = formatContext->bit_rate;
    info.duration = formatContext->duration / (double)AV_TIME_BASE;

    //
    // DECODE AUDIO FRAMES
    //

    // Decode into mono samples
    {
        AVChannelLayout layout;
        av_channel_layout_default(&layout, 1);

        if (!decodeInto(_monoSamples, formatContext, codecContext, swrContext, streamIndex, layout, AV_TARGET_FORMAT)) {
            cleanup();
            return false;
        }
    }

    // Decode into stereo samples if available
    if (info.stereo) {
        AVChannelLayout layout;
        av_channel_layout_default(&layout, 2);

        if (!decodeInto(_stereoSamples, formatContext, codecContext, swrContext, streamIndex, layout, AV_TARGET_FORMAT)) {
            cleanup();
            return false;
        }
    }

    cleanup();
    return true;
}

void AudioData::free()
{
    _monoSamples.clear();
    _stereoSamples.clear();
}

const std::vector<int16_t>& AudioData::monoSamples() const
{
	return _monoSamples;
}

const std::vector<int16_t>& AudioData::stereoSamples() const
{
    return _stereoSamples;
}

std::string AudioData::sourcePath() const
{
    return _sourcePath;
}

bool AudioData::decodeInto(std::vector<int16_t>& samples, AVFormatContext*& formatContext, AVCodecContext*& codecContext, SwrContext*& swrContext, int streamIndex, AVChannelLayout targetLayout, AVSampleFormat targetFormat) const
{
    AVPacket packet;
    av_init_packet(&packet);

    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == streamIndex) {
            if (avcodec_send_packet(codecContext, &packet) < 0) {
                av_packet_unref(&packet);
                return fail("Error sending packet for decoding");
            }

            AVFrame* frame = av_frame_alloc();
            if (!frame) {
                av_packet_unref(&packet);
                return fail("Frame allocation failed");
            }

            while (avcodec_receive_frame(codecContext, frame) == 0) {
                if (av_channel_layout_compare(&frame->ch_layout, &targetLayout) != 0 || frame->format != targetFormat) {
                    if (!swrContext) {
                        if (swr_alloc_set_opts2(&swrContext, &targetLayout, targetFormat, codecContext->sample_rate, &codecContext->ch_layout, codecContext->sample_fmt, codecContext->sample_rate, 0, nullptr) < 0)
                        {
                            av_frame_free(&frame);
                            av_packet_unref(&packet);
                            return fail("Failed to allocate and set SwrContext options");
                        }

                        if (swr_init(swrContext) < 0) {
                            av_frame_free(&frame);
                            av_packet_unref(&packet);
                            return fail("Failed to initialize resampler");
                        }
                    }

                    uint8_t** outputBuffer = nullptr;
                    int outputLinesize = 0;
                    int outputSamples = av_rescale_rnd(frame->nb_samples, codecContext->sample_rate, codecContext->sample_rate, AV_ROUND_UP);

                    if (av_samples_alloc_array_and_samples(&outputBuffer, &outputLinesize,
                        targetLayout.nb_channels, outputSamples, targetFormat, 0) < 0)
                    {
                        av_frame_free(&frame);
                        av_packet_unref(&packet);
                        return fail("Failed to allocate output buffer");
                    }

                    int convertedSamples = swr_convert(swrContext, outputBuffer, outputSamples,
                        (const uint8_t**)frame->extended_data, frame->nb_samples);
                    if (convertedSamples < 0) {
                        av_freep(&outputBuffer[0]);
                        av_freep(&outputBuffer);
                        av_frame_free(&frame);
                        av_packet_unref(&packet);
                        return fail("Failed to convert audio samples");
                    }

                    int dataSize = av_samples_get_buffer_size(nullptr, targetLayout.nb_channels, convertedSamples, targetFormat, 0);
                    samples.insert(samples.end(), (int16_t*)outputBuffer[0], (int16_t*)(outputBuffer[0] + dataSize));

                    av_freep(&outputBuffer[0]);
                    av_freep(&outputBuffer);
                }
                else {
                    if (av_sample_fmt_is_planar((AVSampleFormat)frame->format)) {
                        for (int ch = 0; ch < codecContext->ch_layout.nb_channels; ++ch)
                            samples.insert(samples.end(),
                                reinterpret_cast<int16_t*>(frame->extended_data[ch]),
                                reinterpret_cast<int16_t*>(frame->extended_data[ch]) + frame->nb_samples);
                    }
                    else {
                        int dataSize = av_samples_get_buffer_size(nullptr, codecContext->ch_layout.nb_channels,
                            frame->nb_samples, (AVSampleFormat)frame->format, 1);
                        int sampleCount = dataSize / sizeof(int16_t);
                        samples.insert(samples.end(),
                            reinterpret_cast<int16_t*>(frame->extended_data[0]),
                            reinterpret_cast<int16_t*>(frame->extended_data[0]) + sampleCount);
                    }
                }
            }
            av_frame_free(&frame);
        }
        av_packet_unref(&packet);
    }
}

bool AudioData::fail(std::string info) const
{
	Console::out::warning("Audio Data", info + " of audio data at '" + _sourcePath + "'");
	return false;
}

bool AudioData::validateSource() const
{
	if (!fs::exists(_sourcePath))
		return fail("Could not find audio file");
	return true;
}