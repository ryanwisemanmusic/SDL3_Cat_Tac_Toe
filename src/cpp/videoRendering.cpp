#include <iostream>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_audio.h>

#include "videoRendering.h"

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavutil/channel_layout.h>
    #include <libswresample/swresample.h>
}

#ifndef av_get_default_channel_layout
static inline uint64_t av_get_default_channel_layout(int nb_channels) {
    switch(nb_channels) {
        case 1: return AV_CH_LAYOUT_MONO;
        case 2: return AV_CH_LAYOUT_STEREO;
        case 3: return AV_CH_LAYOUT_2_1;
        case 4: return AV_CH_LAYOUT_QUAD;
        case 5: return AV_CH_LAYOUT_5POINT0_BACK;
        case 6: return AV_CH_LAYOUT_5POINT1;
        default: return 0;
    }
}
#endif

bool loadMP4(const std::string &filename, VideoState &video)
{
    avformat_network_init();
    
    if (avformat_open_input(
        &video.pFormatCtx, filename.c_str(), nullptr, nullptr) !=0)
    {
        std::cout << "Error: Could not open video file " << 
        filename << std::endl;
        return false;
    }

    if (avformat_find_stream_info(video.pFormatCtx, nullptr) < 0)
    {
        std::cout << "Error: Could not find stream information\n";
        return false;
    }

    for (unsigned i = 0; i < video.pFormatCtx->nb_streams; i++)
    {
        if (
            video.pFormatCtx->streams[i]->codecpar->codec_type 
            == AVMEDIA_TYPE_VIDEO)
        {
            video.videoStream = i;
            break;
        } 
    }

    if (video.videoStream == -1)
    {
        std::cout << "Error: No video stream found\n";
        return false;
    }
    
    AVCodecParameters *pCodecParams = 
    video.pFormatCtx->streams[video.videoStream]->codecpar;
    video.pCodec = 
    avcodec_find_decoder(pCodecParams->codec_id);
    if (!video.pCodec)
    {
        std::cout << "Error: Unsupported codec\n";
        return false;
    }

    video.pCodecCtx = avcodec_alloc_context3(video.pCodec);
    if (avcodec_parameters_to_context(video.pCodecCtx, pCodecParams) < 0)
    {
        std::cout << "Error: Could not copy codec parameters\n";
        return false;
    }
    
    if (avcodec_open2(video.pCodecCtx, video.pCodec, nullptr) < 0)
    {
        std::cout << "Error: Could not open codec\n";
        return false;
    }
    
    std::cout << "Sucessfully loaded MP4: " << filename << std::endl;
    return true;
}

bool loadAudio(const std::string &filename, VideoState &video)
{
    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = 44100;              // Desired sample rate
    desiredSpec.format = SDL_AUDIO_S16;      // Signed 16-bit format
    desiredSpec.channels = 2;              // Stereo output

    // Store the desired audio specification in VideoState.
    video.audioSpec = desiredSpec;

    // Find the audio stream index.
    for (unsigned i = 0; i < video.pFormatCtx->nb_streams; i++) {
        if (video.pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            video.audioStreamIndex = i;
            break;
        }
    }
    if (video.audioStreamIndex == -1) {
        SDL_Log("Error: No audio stream found\n");
        return false;
    }

    AVCodecParameters *pCodecParams = video.pFormatCtx->streams[video.audioStreamIndex]->codecpar;
    video.pAudioCodec = avcodec_find_decoder(pCodecParams->codec_id);
    if (!video.pAudioCodec) {
        SDL_Log("Error: Unsupported audio codec\n");
        return false;
    }

    video.pAudioCodecCtx = avcodec_alloc_context3(video.pAudioCodec);
    if (avcodec_parameters_to_context(video.pAudioCodecCtx, pCodecParams) < 0) {
        SDL_Log("Error: Could not copy audio codec parameters\n");
        return false;
    }
    if (avcodec_open2(video.pAudioCodecCtx, video.pAudioCodec, nullptr) < 0) {
        SDL_Log("Error: Could not open audio codec\n");
        return false;
    }

    std::cout << "Successfully loaded audio: " << filename << std::endl;
    return true;
}

bool decodeAndQueueAudio(VideoState &video)
{
    AVPacket packet;
    AVFrame* frame = av_frame_alloc();
    bool audioQueued = false;

    // Set up the desired SDL audio specification (S16, stereo, 44100 Hz).
    SDL_AudioSpec desiredSpec = video.audioSpec;
    desiredSpec.format   = SDL_AUDIO_S16;
    desiredSpec.channels = 2;
    desiredSpec.freq     = 44100;

    // Create a single SDL_AudioStream with explicit source and destination formats.
    SDL_AudioStream* audioStream = SDL_CreateAudioStream(&desiredSpec, &desiredSpec);
    if (!audioStream) {
         SDL_Log("SDL_CreateAudioStream error: %s", SDL_GetError());
         av_frame_free(&frame);
         return false;
    }

    // Loop over packets.
    while (av_read_frame(video.pFormatCtx, &packet) >= 0)
    {
        if (packet.stream_index == video.audioStreamIndex)
        {
            if (avcodec_send_packet(video.pAudioCodecCtx, &packet) < 0) {
                av_packet_unref(&packet);
                continue;
            }
            
            // Process all available decoded frames.
            while (avcodec_receive_frame(video.pAudioCodecCtx, frame) >= 0)
            {
                int nb_samples    = frame->nb_samples;
                if (nb_samples <= 0) continue;
                int in_channels   = video.pAudioCodecCtx->ch_layout.nb_channels;
                int in_sample_rate= video.pAudioCodecCtx->sample_rate;
                enum AVSampleFormat in_format = video.pAudioCodecCtx->sample_fmt;

                // Allocate and initialize the resampling context.
                SwrContext *swrCtx = swr_alloc();
                if (!swrCtx) {
                    SDL_Log("Failed to allocate SwrContext");
                    break;
                }

                // Set up destination and source channel layouts.
                AVChannelLayout dst_layout;
                av_channel_layout_default(&dst_layout, desiredSpec.channels);
                AVChannelLayout src_layout;
                av_channel_layout_default(&src_layout, in_channels);

                // Configure the SwrContext.
                if (swr_alloc_set_opts2(&swrCtx,
                    &dst_layout,               // Destination layout pointer
                    AV_SAMPLE_FMT_S16,         // Destination sample format
                    desiredSpec.freq,          // Destination sample rate
                    &src_layout,               // Source layout pointer
                    in_format,                 // Source sample format
                    in_sample_rate,            // Source sample rate
                    0, nullptr) < 0)
                {
                    SDL_Log("Failed to set SwrContext options");
                    swr_free(&swrCtx);
                    break;
                }

                if (swr_init(swrCtx) < 0) {
                    SDL_Log("Failed to initialize SwrContext: %s", SDL_GetError());
                    swr_free(&swrCtx);
                    break;
                }

                // Allocate a buffer for the converted (resampled) data.
                // Use av_samples_alloc_array_and_samples to create an array of pointers.
                uint8_t **dst_data = nullptr;
                int dst_linesize = 0;
                int ret = av_samples_alloc_array_and_samples(
                    &dst_data, &dst_linesize, desiredSpec.channels,
                    nb_samples, AV_SAMPLE_FMT_S16, 0);
                if (ret < 0) {
                    SDL_Log("Failed to allocate converted audio buffer");
                    swr_free(&swrCtx);
                    break;
                }
                
                // Perform resampling.
                int sampleCount = swr_convert(swrCtx, dst_data, nb_samples,
                                              (const uint8_t**)frame->data, nb_samples);
                if (sampleCount < 0) {
                    SDL_Log("Error during resampling");
                    av_freep(&dst_data[0]);
                    av_freep(&dst_data);
                    swr_free(&swrCtx);
                    break;
                }
                
                // Calculate the size in bytes of the converted data.
                int convertedDataSize = sampleCount * desiredSpec.channels *
                                         (SDL_AUDIO_BITSIZE(desiredSpec.format) / 8);
                
                // Queue the resampled audio data into the SDL audio stream.
                if (!SDL_PutAudioStreamData(audioStream, dst_data[0], convertedDataSize)) {
                    SDL_Log("SDL_PutAudioStreamData error: %s", SDL_GetError());
                } else {
                    audioQueued = true;
                }
                
                // Clean up the conversion resources.
                av_freep(&dst_data[0]);
                av_freep(&dst_data);
                swr_free(&swrCtx);
                
                if (audioQueued)
                    break;
            }
        }
        av_packet_unref(&packet);
        if (audioQueued)
            break;
    }

    if (!SDL_FlushAudioStream(audioStream)) {
         SDL_Log("SDL_FlushAudioStream error: %s", SDL_GetError());
    }

    SDL_DestroyAudioStream(audioStream);
    av_frame_free(&frame);
    return audioQueued;
}

