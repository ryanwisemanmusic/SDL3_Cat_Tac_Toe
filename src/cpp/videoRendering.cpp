#include <iostream>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "videoRendering.h"

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

bool loadMP4(const std::string &filename, VideoState &video)
{
    avformat_network_init();
    
    if (avformat_open_input(
        &video.pFormatCtx, filename.c_str(), nullptr, nullptr) !=0)
    {
        std::cerr << "Error: Could not open video file " << 
        filename << std::endl;
        return false;
    }

    if (avformat_find_stream_info(video.pFormatCtx, nullptr) < 0)
    {
        std::cerr << "Error: Could not find stream information\n";
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
        std::cerr << "Error: No video stream found\n";
        return false;
    }
    
    AVCodecParameters *pCodecParams = 
    video.pFormatCtx->streams[video.videoStream]->codecpar;
    video.pCodec = 
    avcodec_find_decoder(pCodecParams->codec_id);
    if (!video.pCodec)
    {
        std::cerr << "Error: Unsupported codec\n";
        return false;
    }

    video.pCodecCtx = avcodec_alloc_context3(video.pCodec);
    if (avcodec_parameters_to_context(video.pCodecCtx, pCodecParams) < 0)
    {
        std::cerr << "Error: Could not copy codec parameters\n";
        return false;
    }
    
    if (avcodec_open2(video.pCodecCtx, video.pCodec, nullptr) < 0)
    {
        std::cerr << "Error: Could not open codec\n";
        return false;
    }
    
    std::cout << "Sucessfully loaded MP4: " << filename << std::endl;
    return true;
}


