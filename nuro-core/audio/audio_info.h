#pragma once

#include <string>
#include <cstdint>
#include <unordered_map>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

struct AudioInfo {

	// Path to audio file
	std::string path = "None";

	//  Audio files name
	std::string name = "Unnamed";

	// File format
	std::string fileFormat = "Invalid";

	// Audio codec
	std::string codec = "Invalid";

	// Samples per second
	int32_t sampleRate = 0;

	// Number of audio channels
	int32_t nChannels = 0;

    // Format of the audio samples within the file
    AVSampleFormat sampleFormat;

	// Bitrate in bit/s
	int32_t bitrate = 0;

	// Duration in seconds
	double duration = 0.0;

	// Metadata
	std::unordered_map<std::string, std::string> metadata;

    // Convert sample format to string
    std::string formatToStr(AVSampleFormat sampleFormat) const {
        switch (sampleFormat) {
        case AV_SAMPLE_FMT_U8:
            return "Unsigned 8-bit Integer";
        case AV_SAMPLE_FMT_S16:
            return "Signed 16-bit Integer";
        case AV_SAMPLE_FMT_S32:
            return "Signed 32-bit Integer";
        case AV_SAMPLE_FMT_FLT:
            return "32-bit Floating Point";
        case AV_SAMPLE_FMT_DBL:
            return "64-bit Floating Point";
        case AV_SAMPLE_FMT_U8P:
            return "Unsigned 8-bit Integer Planar";
        case AV_SAMPLE_FMT_S16P:
            return "Signed 16-bit Integer Planar";
        case AV_SAMPLE_FMT_S32P:
            return "Signed 32-bit Integer Planar";
        case AV_SAMPLE_FMT_FLTP:
            return "32-bit Floating Point Planar";
        case AV_SAMPLE_FMT_DBLP:
            return "64-bit Floating Point Planar";
        case AV_SAMPLE_FMT_S64:
            return "Signed 64-bit Integer";
        case AV_SAMPLE_FMT_S64P:
            return "Signed 64-bit Integer Planar";
        default:
            return "Unknown Format";
        }
    }

};