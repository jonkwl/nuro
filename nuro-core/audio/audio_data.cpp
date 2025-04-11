#include "audio_data.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <filesystem>

#include <utils/console.h>
#include <utils/fsutil.h>

namespace fs = std::filesystem;

AudioData::AudioData() : _sourcePath(),
_monoSamples(nullptr),
_multichannelSamples(nullptr)
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
    // MAKE SURE ANY EXISTING DATA IS FREED
    //

    free();

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
    info.fileFormat = std::string(formatContext->iformat->name);
    info.codec = codec->name;
    info.sampleRate = codecContext->sample_rate;
    info.nChannels = codecContext->ch_layout.nb_channels;
    info.sampleFormat = codecContext->sample_fmt;
    info.bitrate = formatContext->bit_rate;
    info.duration = formatContext->duration / (double)AV_TIME_BASE;

    //
    // FETCH METADATA
    //

    AVDictionary* metaDictionary = formatContext->metadata;
    if (metaDictionary != nullptr) {
        AVDictionaryEntry* entry = nullptr;
        while ((entry = av_dict_get(metaDictionary, "", entry, AV_DICT_IGNORE_SUFFIX)) != nullptr) {
            info.metadata[entry->key] = entry->value;
        }
    }

    //
    // DECODE MONO SAMPLES
    //

    {
        // Mono
        _monoSamples = new AudioSamplesBuffer<int16_t>(info.sampleRate, getLayout_by_nChannels(1), AV_SAMPLE_FMT_S16, AL_FORMAT_MONO16);
        if (!decodeInto(_monoSamples, formatContext, codecContext, swrContext, streamIndex)) {
            cleanup();
            return fail("Failed decoding mono samples");
        }
    }
    
    //
    // DECODE MULTICHANNEL
    //

    if (info.nChannels == 2) {
        // Stereo
        _multichannelSamples = new AudioSamplesBuffer<int16_t>(info.sampleRate, getLayout_by_nChannels(2), AV_SAMPLE_FMT_S16, AL_FORMAT_STEREO16);
        if (!decodeInto(_multichannelSamples, formatContext, codecContext, swrContext, streamIndex)) {
            cleanup();
            return fail("Failed decoding stereo samples");
        }
    }

    // Note: Channel layouts with >2 channels can't always be determined by the audio file or stream itself, therefore the 
    // channel layout for an audio with >2 channels should be enabled and chosen manually later to prevent mismatches.
    // 
    // --> For now any 4-channel audio gets decoded into B-format 3D
    else if (info.nChannels == 4) {
        // B-format 3D
        _multichannelSamples = new AudioSamplesBuffer<int16_t>(info.sampleRate, getLayout_by_mask(AV_CH_LAYOUT_4POINT0), AV_SAMPLE_FMT_S16, AL_FORMAT_BFORMAT3D_16);
        if (!decodeInto(_multichannelSamples, formatContext, codecContext, swrContext, streamIndex)) {
            cleanup();
            return fail("Failed decoding 4-channel bformat samples");
        }
    }

    cleanup();
    return true;
}

void AudioData::free()
{
    if (_monoSamples) 
        delete _monoSamples;

    if (_multichannelSamples) 
        delete _multichannelSamples;

    _monoSamples = nullptr;
    _multichannelSamples = nullptr;
}

AudioSamples* AudioData::monoSamples() const
{
	return _monoSamples;
}

AudioSamples* AudioData::multichannelSamples() const
{
    return _multichannelSamples;
}

std::string AudioData::sourcePath() const
{
    return _sourcePath;
}

bool AudioData::decodeInto(AudioSamples* samples, AVFormatContext*& formatContext, AVCodecContext*& codecContext, SwrContext*& swrContext, int streamIndex) const
{
    //
    // MAKE SURE SAMPLES TO BE FILLED EXIST
    //

    if (!samples) return false;

    //
    // RESET STREAM POSITION TO BEGINNING
    //

    if (av_seek_frame(formatContext, streamIndex, 0, AVSEEK_FLAG_BACKWARD) < 0) {
        return fail("Failed to seek to the beginning of the stream");
    }

    avcodec_flush_buffers(codecContext);

    if (swrContext) {
        swr_free(&swrContext);
        swrContext = nullptr;
    }

    //
    // DECODE FRAMES
    //

    AVChannelLayout targetLayout = samples->getAvTargetLayout();
    AVSampleFormat targetFormat = samples->getAvTargetFormat();

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

                //
                // TARGET LAYOUT OR FORMAT NOT MATCHING
                //

                if (av_channel_layout_compare(&frame->ch_layout, &targetLayout) != 0 || frame->format != targetFormat) {
                   
                    //
                    // NO SWR CONTEXT --> CREATE SWR CONTEXT
                    //

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

                    //
                    // DECODE SAMPLES
                    //

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
                    samples->insertSamples(outputBuffer[0], outputBuffer[0] + dataSize);

                    av_freep(&outputBuffer[0]);
                    av_freep(&outputBuffer);
                }

                //
                // TARGET LAYOUT OR FORMAT MATCHING
                //

                else {

                    //
                    // DECODE PLANAR SAMPLES
                    //

                    if (av_sample_fmt_is_planar((AVSampleFormat)frame->format)) {
                        for (int ch = 0; ch < codecContext->ch_layout.nb_channels; ++ch) {
                            int16_t* channelData = reinterpret_cast<int16_t*>(frame->extended_data[ch]);
                            samples->insertSamples(channelData, channelData + frame->nb_samples);
                        }
                    }

                    //
                    // DECODE NON PLANAR SAMPLES
                    //

                    else {
                        int dataSize = av_samples_get_buffer_size(nullptr, codecContext->ch_layout.nb_channels,
                            frame->nb_samples, (AVSampleFormat)frame->format, 1);
                        int sampleCount = dataSize / sizeof(int16_t); 
                        int16_t* buffer = reinterpret_cast<int16_t*>(frame->extended_data[0]);
                        samples->insertSamples(buffer, buffer + sampleCount);
                    }
                }
            }
            av_frame_free(&frame);
        }
        av_packet_unref(&packet);
    }

    return true;
}

AVChannelLayout AudioData::getLayout_by_nChannels(int32_t nChannels) const
{
    AVChannelLayout layout;
    av_channel_layout_default(&layout, nChannels);
    return layout;
}

AVChannelLayout AudioData::getLayout_by_mask(uint64_t mask) const
{
    AVChannelLayout layout;
    av_channel_layout_from_mask(&layout, mask);
    return layout;
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