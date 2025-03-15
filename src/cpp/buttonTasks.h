#ifndef BUTTONTASKS
#define BUTTONTASKS

#include <iostream>
#include <string>
#include <cstdint>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

//Global variables
SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font* font = nullptr;

void drawButton(SDL_Renderer *renderer, const Button *button);
void processButtonEvents(
    SDL_Event *event, Button *button, SDL_Renderer *renderer);
void renderButtonText(
    const char* message, int x, int y, SDL_Color color);


struct Button 
{
    SDL_Rect rect;
    SDL_Color color;
    bool pressed;
    void (*callback)(void*);
    void* userdata; 
};


#endif