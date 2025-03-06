/*
Author: Ryan Wiseman

This is a barebones approach to windowing via SDL3. Any
intenseive windowing required will require some major refactoring
*/
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>

// Global variables
SDL_Window* gWindow{ nullptr };
SDL_Surface* gScreenSurface{ nullptr };
SDL_Surface* gHelloWorld{ nullptr };

// Constexpr integers
constexpr int kScreenWidth{ 1280 };
constexpr int kScreenHeight{ 720 };

// Function prototypes
bool init();
bool loadMedia();
void close();

// Cocoa functions
extern "C" void cocoaBaseMenuBar();
extern "C" void openSDLWindowAboutMenu();

int main() {
    // Initialize
    if( !init() )
    {
        SDL_Log( "Unable to initialize program!\n" );
        return 1;  // Return directly instead of using exitCode
    }
    else
    {
        cocoaBaseMenuBar();
        // Load media
        if( !loadMedia() )
        {
            SDL_Log( "Unable to load media!\n" );
            return 2;  // Return directly instead of using exitCode
        }
    }

    // Event loop to keep the window open
    bool done = false;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }

        // Here you can also render to the screen or update the window, if needed
        SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);  // Blit the image to the screen
        SDL_UpdateWindowSurface(gWindow);  // Update the window surface
    }

    close();  // Clean up resources before closing
    return 0;
}

bool init()
{
    // Initialization flag
    bool success{ true };

    // Initialize SDL
    if( !SDL_Init( SDL_INIT_VIDEO ) )
    {
        SDL_Log( "SDL could not initialize! SDL error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        // Create window
        if( gWindow = SDL_CreateWindow( "AtraxiaSDK", kScreenWidth, kScreenHeight, 0 ); gWindow == nullptr )
        {
            SDL_Log( "Window could not be created! SDL error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            // Get window surface
            gScreenSurface = SDL_GetWindowSurface( gWindow );
        }
    }

    return success;
}

bool loadMedia()
{
    // File loading flag
    bool success{ true };

    // Load image
    std::string imagePath{ "/Volumes/2023 Drive/SDL3_Cat_Tac_Toe/assets/images/downwardspiral.png" };
    gHelloWorld = IMG_Load(imagePath.c_str());  // Use IMG_Load for non-BMP images
    if( gHelloWorld == nullptr )
    {
        SDL_Log( "Unable to load image %s! SDL Error: %s\n", imagePath.c_str(), SDL_GetError() );
        success = false;
    }

    return success;
}

void close()
{
    // Clean up surface
    SDL_DestroySurface( gHelloWorld );
    gHelloWorld = nullptr;
    
    // Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = nullptr;
    gScreenSurface = nullptr;

    // Quit SDL subsystems
    SDL_Quit();
}
