/*
Author: Ryan Wiseman
*/

//Library headers
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_audio.h>
#include <sqlite3.h>
#include <iostream>

#include <array>

//App headers
#include "gameScores.h"
#include "SDLColors.h"
#include "screenScenes.h"
#include "videoRendering.h"

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

//Global variables
SDL_Window *window;
SDL_Renderer *renderer;
VideoState video;

constexpr int ScreenWidth = 600;
constexpr int ScreenHeight = 600;
constexpr int SprightSize = 200;

TTF_Font* font = nullptr;

enum class Player
{
    NONE, X, O
};

//Player globals
std::array<std::array<Player, 3>, 3> board{};
Player Player1 = Player::X;
Player Player2 = Player::O;

int scorePlayer1 = 0;
int scorePlayer2 = 0;

int player1WinCount = 0;
int player2WinCount = 0;

SceneState currentScene = SceneState::MAIN_MENU;

//External calls
bool decodeAndQueueAudio(VideoState &video);

//Function prototypes
bool init();
bool initSDL_ttf();
bool initAudio(VideoState &video);
bool initMP4(const std::string &filename, VideoState &video);
void render();
void renderText(const char* message, int x, int y, SDL_Color color);
SDL_Texture* getNextFrame(VideoState &video, SDL_Renderer* renderer);
void audioCallback(void *userdata, Uint8 *stream, int len);
void handleEvents(bool& done);
bool checkWin(Player player);
void resetBoard();
void close();

extern "C" void cocoaBaseMenuBar();

int main(int argc, char* argv[]) {
    
    (void)argc;
    (void)argv;
    
    /*
    It took me about 12 hours to fix the problem with my call to
    bool init. So this is some good progress!!!!
    */
    if (!init()) {
        SDL_Log("Unable to initialize program!\n");
        return 1;
    }

    // Add Cocoa base menu bar
    cocoaBaseMenuBar();

    bool done = false;

    SDL_Renderer* renderer = SDL_GetRenderer(window);
    if (!renderer) {
        SDL_Log("Failed to get renderer!\n");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Main loop for window event handling
    while (!done) {
        handleEvents(done);
        render();
    }

    // Cleanup
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

bool init()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    
    window = SDL_CreateWindow(
        "SDL3 Cat-Tac-Toe", ScreenWidth,
        ScreenHeight, SDL_WINDOW_OPENGL);
    
    if (window == NULL)
    {
        SDL_Log("Window can't be creted! SDL error: %s\n",
        SDL_GetError());
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, 0);
    if (!renderer)
    {
        SDL_Log("Renderer could not be created! SDL error: %s\n",
        SDL_GetError());
        SDL_Quit();
        return false;
    }
    /*
    if (SDL_SetRenderVSync(renderer, true) != 0)
    {
        SDL_Log("Failed to enable VSync! SDL: Error %s\n",
        SDL_GetError());
    }
    */
    return true;
}

bool initSDL_ttf()
{
    if (!TTF_Init()) {
        SDL_Log("TTF_Init failed");
        return false;
    }

    std::string fontPath = "assets/fonts/ArianaVioleta.ttf";

    font = TTF_OpenFont(fontPath.c_str(), 20);
    if (!font) 
    {
        SDL_Log("Cannot load font");
        return false;
    }

    return true;
}

bool initMP4(const std::string &filename, VideoState &video)
{
    if (loadMP4(filename, video)) 
    {
        std::cout << "MP4 file loaded successfully: " << filename << std::endl;
        std::cout << "Video Stream Index: " << video.videoStream << std::endl;
        std::cout << "Codec: " << video.pCodec->name << std::endl;
        std::cout << "Resolution: " << video.pCodecCtx->width << 
                     "x" << video.pCodecCtx->height << std::endl;

        return true;
    }
    else
    {
        std::cerr << "Failed to load MP4 file: " << filename << std::endl;
        return false;
    }
}

bool initAudio(VideoState &video)
{
    SDL_AudioSpec wantedSpec;

    // Define the desired audio format (16-bit stereo, 44.1 kHz, etc.)
    wantedSpec.freq = 44100;
    wantedSpec.format = SDL_AUDIO_S16;
    wantedSpec.channels = 2;
    // In SDL3, the callback and userdata members have been removed.
    // You must use SDL_QueueAudio() to supply audio data instead.
    std::cout << "Audio initialized" << std::endl;
    // Open the audio device.
    video.audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &wantedSpec);
    if (video.audioDevice == 0) {
        SDL_Log("Error: Could not open audio device: ");
        SDL_GetError();
        return false;
    }

    SDL_PauseAudioDevice(video.audioDevice);
    return true;
}

void render()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    if (currentScene == SceneState::MAIN_MENU)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, nullptr);
        renderText("Cat Tac Toe", 225, 250, cMagenta);
    }
    else if (currentScene == SceneState::GAME)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (int i = 1; i < 3; i++)
        {
            SDL_RenderLine(renderer, i * SprightSize, 0, i * SprightSize, ScreenHeight);
            SDL_RenderLine(renderer, 0, i * SprightSize, ScreenWidth, i * SprightSize);
        }

        for (int row = 0; row < 3; ++row)
        {
            for (int col = 0; col < 3; ++col)
            {
                int x = col * SprightSize;
                int y = row * SprightSize;

                if (board[row][col] == Player::X)
                {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_RenderLine(renderer, x + SprightSize - 20, y + 20, x + 20, y + SprightSize - 20);
                    SDL_RenderLine(renderer, x + 20, y + 20, x + SprightSize - 20, y + SprightSize - 20);
                }
                else if (board[row][col] == Player::O)
                {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    SDL_FRect rect {
                        static_cast<float>(x + 20), 
                        static_cast<float>(y + 20),
                        static_cast<float>(SprightSize - 40), 
                        static_cast<float>(SprightSize - 40)
                    };
                    SDL_RenderRect(renderer, &rect);
                }
            }
        }
    }
    else if (currentScene == SceneState::END_SCREEN)
    {
        static bool videoInitialized = false;
        static SDL_Texture* videoTexture = nullptr;
        static uint32_t lastFrameTime = 0;
        static double frameDelay = 33.33; // Default 30fps
        
        if (!videoInitialized)
        {
            std::string mp4File = "assets/video/NeverGonna.mp4";
            
            // Initialize video first
            if (!initMP4(mp4File, video)) {
                std::cerr << "Failed to initialize video\n";
                return;
            }
            
            // Calculate frame delay AFTER video initialization
            if (video.pFormatCtx && video.videoStream >= 0) {
                AVStream* stream = video.pFormatCtx->streams[video.videoStream];
                if (stream->avg_frame_rate.den != 0) {
                    frameDelay = 1000.0 * stream->avg_frame_rate.den / stream->avg_frame_rate.num;
                }
            }
            
            // Initialize audio components
            if (!initAudio(video)) {
                std::cerr << "Failed to initialize audio\n";
                return;
            }
            
            if (!loadAudio(mp4File, video)) {
                std::cerr << "Failed to load audio\n";
                return;
            }
            
            if (!decodeAndQueueAudio(video)) {
                std::cerr << "Failed to queue audio\n";
                return;
            }
            
            if (video.audioStream) {
                SDL_BindAudioStream(video.audioDevice, video.audioStream);
                SDL_PauseAudioDevice(video.audioDevice); // 0 = unpause
            }

            videoInitialized = true;
            SDL_PauseAudioDevice(video.audioDevice); // Start audio playback
        }

        // Frame timing control
        uint32_t currentTime = SDL_GetTicks();
        if (currentTime - lastFrameTime >= frameDelay) {
            SDL_Texture* nextFrame = getNextFrame(video, renderer);
            if (nextFrame) {
                if (videoTexture) SDL_DestroyTexture(videoTexture);
                videoTexture = nextFrame;
            }
            lastFrameTime = currentTime;
        }

        // Render video frame
        if (videoTexture) {
            SDL_RenderTexture(renderer, videoTexture, nullptr, nullptr);
        }

        // Render text overlays
        renderText("GAME OVER", 200, 250, cMagenta);
        renderText("Click to play again!", 175, 400, cMagenta);
    }

    SDL_RenderPresent(renderer);
}

void renderText(const char* message, int x, int y, SDL_Color color)
{
    
    TTF_Init();
    std::string fontPath = "assets/fonts/ArianaVioleta.ttf";

    font = TTF_OpenFont(fontPath.c_str(), 50);
    if (!font) 
    {
        SDL_Log("Cannot load font!");
    }
    size_t messageLength = strlen(message);

    SDL_Surface* textSurface = 
    TTF_RenderText_Solid(font, message, messageLength, color);

    SDL_Texture* textTexture = 
    SDL_CreateTextureFromSurface(renderer, textSurface);

    int textW = textSurface->w; 
    int textH = textSurface->h;
    SDL_DestroySurface(textSurface);

    if (!textTexture)
    {
        SDL_Log("Texture creation failed!");
        return;
    }

    SDL_FRect destRect = 
    { 
        static_cast<float>(x), static_cast<float>(y), 
        static_cast<float>(textW), static_cast<float>(textH) 
    };

    SDL_RenderTexture(renderer, textTexture, nullptr, &destRect);

    SDL_DestroyTexture(textTexture);
}

SDL_Texture* getNextFrame(VideoState &video, SDL_Renderer* renderer) 
{
    if (!video.pFormatCtx || !video.pCodecCtx) return nullptr;
    
    AVPacket packet;
    AVFrame *frame = av_frame_alloc();

    while (av_read_frame(video.pFormatCtx, &packet) >= 0)
    {
        if (packet.stream_index == video.videoStream)
        {
            avcodec_send_packet(video.pCodecCtx, &packet);
            if (avcodec_receive_frame(video.pCodecCtx, frame) == 0)
            {
                if (!video.swsCtx)
                {
                    video.swsCtx = sws_getContext(
                        video.pCodecCtx->width, video.pCodecCtx->height,
                        video.pCodecCtx->pix_fmt,
                        video.pCodecCtx->width, video.pCodecCtx->height,
                        AV_PIX_FMT_RGB24,
                        SWS_BILINEAR, nullptr, nullptr, nullptr
                    );
                    if (!video.swsCtx)
                    {
                        std::cerr << "Failed to create SwsContext\n";
                        av_frame_free(&frame);
                        av_packet_unref(&packet);
                        return nullptr;
                    }
                }

                if (!video.pFrameRGB)
                {
                    video.pFrameRGB = av_frame_alloc();
                    int numBytes = av_image_get_buffer_size(
                        AV_PIX_FMT_RGB24, video.pCodecCtx->width,
                        video.pCodecCtx->height, 1);
                    if (numBytes < 0)
                    {
                        std::cerr << "Failed to calculate buffer size\n";
                        av_frame_free(&frame);
                        av_packet_unref(&packet);
                        return nullptr;
                    }
                    video.buffer = (uint8_t*) av_malloc(numBytes * sizeof(uint8_t));
                    if (!video.buffer)
                    {
                        std::cerr << "Failed to allocate buffer\n";
                        av_frame_free(&frame);
                        av_packet_unref(&packet);
                        return nullptr;
                    }
                    av_image_fill_arrays(
                        video.pFrameRGB->data, video.pFrameRGB->linesize,
                        video.buffer, AV_PIX_FMT_RGB24,
                        video.pCodecCtx->width, video.pCodecCtx->height, 1
                    );
                }

                int ret = sws_scale(
                    video.swsCtx, frame->data, frame->linesize,
                    0, video.pCodecCtx->height,
                    video.pFrameRGB->data, video.pFrameRGB->linesize
                );
                if (ret < 0)
                {
                    std::cerr << "sws_scale failed\n";
                    av_frame_free(&frame);
                    av_packet_unref(&packet);
                    return nullptr;
                }

                // Create SDL_Surface
                SDL_Surface* surface = SDL_CreateSurface(
                    video.pCodecCtx->width,
                    video.pCodecCtx->height,
                    SDL_PIXELFORMAT_RGB24
                );
                if (!surface)
                {
                    std::cerr << "Failed to create SDL surface: " << SDL_GetError() << std::endl;
                    av_frame_free(&frame);
                    av_packet_unref(&packet);
                    return nullptr;
                }

                // Copy pixel data line by line
                uint8_t* srcData = video.pFrameRGB->data[0];
                int srcPitch = video.pFrameRGB->linesize[0];
                uint8_t* dstData = (uint8_t*)surface->pixels;
                int dstPitch = surface->pitch;
                int height = video.pCodecCtx->height;
                int width = std::min(srcPitch, dstPitch); // Copy the minimum of both pitches

                for (int y = 0; y < height; y++)
                {
                    memcpy(dstData + y * dstPitch, srcData + y * srcPitch, width);
                }

                // Convert Surface to Texture
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_DestroySurface(surface);

                av_frame_free(&frame);
                av_packet_unref(&packet);

                return texture;
            }
        }
        av_packet_unref(&packet);
    }

    av_frame_free(&frame);
    return nullptr;
}

void handleEvents(bool& done)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            done = true;
        }
        else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            int x = event.button.x;
            int y = event.button.y;

            if (currentScene == SceneState::MAIN_MENU)
            {
                currentScene = SceneState::GAME;
            }
            else if (currentScene == SceneState::GAME)
            {
                if (x < 50 && y < 50) 
                {
                    currentScene = SceneState::END_SCREEN;
                    return;
                }

                int boardX = x / SprightSize;
                int boardY = y / SprightSize;

                if (boardX >= 0 && boardX < 3 && boardY >= 0 && boardY < 3)
                {
                    if (board[boardY][boardX] == Player::NONE)
                    {
                        board[boardY][boardX] = Player1;

                        if (checkWin(Player1))
                        {
                            std::string winnerName = 
                            (Player1 == Player::X) ? "Player 1" : "Player 2";
                            SDL_Log("%s wins!", winnerName.c_str());
                            SDL_Delay(1000);
                            resetBoard();

                            DatabaseManager dbManager("scoresDatabase.db");

                            if (dbManager.insertTestScore(winnerName, 1)) 
                            {
                                if (Player1 == Player::X) 
                                    ++player1WinCount;
                                else 
                                    ++player2WinCount;

                                std::cout << "Updated score for: " << 
                                winnerName << " successfully!" << std::endl;
                            }
                            else
                            {
                                std::cerr << "Failed to update score for: " << 
                                winnerName << std::endl;
                            }

                            std::cout << "Current scores in the database:" << std::endl;
                            dbManager.queryScores();

                            if (player1WinCount >= 3 || player2WinCount >= 3)
                            {
                                currentScene = SceneState::END_SCREEN;
                                return;
                            }

                            return;
                        }

                        Player1 = (Player1 == Player::X) ? Player::O : Player::X;
                    }
                }
            }
            else if (currentScene == SceneState::END_SCREEN)
            {
                player1WinCount = 0;
                player2WinCount = 0;
                currentScene = SceneState::MAIN_MENU;

                // Proper cleanup
                avformat_close_input(&video.pFormatCtx);
                avcodec_free_context(&video.pCodecCtx);
                avcodec_free_context(&video.pAudioCodecCtx);
                if (video.swsCtx) {
                    sws_freeContext(video.swsCtx);
                    video.swsCtx = nullptr;
                }
                if (video.audioStream) {
                    SDL_DestroyAudioStream(video.audioStream);
                    video.audioStream = nullptr;
                }
                SDL_CloseAudioDevice(video.audioDevice);
            }
    }
}
}


bool checkWin(Player player)
{
    for (int i = 0; i < 3; i++)
    {
        if ((board[i][0] == player && board[i][1] == player &&
        board[i][2] == player) || (board[0][i] == player && 
        board[1][i] == player && board[2][i] == player))
        {
            return true;
        }
    }
    return (board[0][0] == player && board[1][1] == player && 
            board[2][2] == player) || 
            (board[0][2] == player && board [1][1] == player &&
            board[2][0] == player);
    
}

void resetBoard()
{
    for (auto& row : board)
    {
        row.fill(Player::NONE);
    }
    Player1 = Player::X;
    
}

void close()
{
    if (font)
    {
        TTF_CloseFont(font);
        font = nullptr;
    }
    
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}