/*
Author: Ryan Wiseman

This is a barebones approach to windowing via SDL3. Any
intenseive windowing required will require some major refactoring
*/
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <array>

constexpr int kScreenWidth = 600;
constexpr int kScreenHeight = 600;
constexpr int kCellSize = 200;

extern "C" void cocoaBaseMenuBar();

int main(int argc, char* argv[]) {
    (void)argc; 
    (void)argv; 

    SDL_Window *window;  
    bool done = false;

    // Initialize SDL video subsystem
    SDL_Init(SDL_INIT_VIDEO);  

    // Create the window with SDL_WINDOW_OPENGL flag
    window = SDL_CreateWindow(
        "An SDL3 window",
        kScreenWidth, kScreenHeight, 
        SDL_WINDOW_OPENGL 
    );

    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Add Cocoa base menu bar
    cocoaBaseMenuBar();

    // Main loop for window event handling
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }

        SDL_Renderer* renderer = SDL_CreateRenderer(window, 0);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }

    // Cleanup
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


