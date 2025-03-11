#ifndef VIDEO_RENDERING_H
#define VIDEO_RENDERING_H

#include <string>
#include <SDL3/SDL.h>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}

struct VideoState
{
    //Video Component
    AVFormatContext *pFormatCtx = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    const AVCodec *pCodec = nullptr;
    AVFrame *pFrame = nullptr;
    AVFrame *pFrameRGB = nullptr;
    SwsContext *swsCtx = nullptr;
    uint8_t *buffer = nullptr;  // Store RGB buffer
    int videoStream = -1;
    //Audio Component
    int audioStream = -1;
    AVCodecContext *pAudioCodecCtx = nullptr;
    AVCodec *pAudioCodec = nullptr;
    SDL_AudioDeviceID audioDevice = 0;

    VideoState() : pFormatCtx(nullptr), pCodecCtx(nullptr), pCodec(nullptr),
                   pFrame(nullptr), pFrameRGB(nullptr), swsCtx(nullptr),
                   buffer(nullptr), videoStream(-1) {}

    ~VideoState()
    {
        if (buffer) av_free(buffer);
        if (pFrameRGB) av_frame_free(&pFrameRGB);
        if (pFrame) av_frame_free(&pFrame);
        if (pCodecCtx) avcodec_free_context(&pCodecCtx);
        if (pFormatCtx) avformat_close_input(&pFormatCtx);
        if (swsCtx) sws_freeContext(swsCtx);
    }
};


bool loadMP4(const std::string &filename, VideoState &video);
bool loadAudio(const std::string &filename, VideoState &video);

#endif