#ifndef BUTTONTASKS
#define BUTTONTASKS

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

// Extern declarations
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;

struct Button 
{
    SDL_Rect rect;
    SDL_Color color;
    bool pressed;
    void (*callback)(void*);
    void* userdata; 
};

void drawButton(SDL_Renderer* renderer, const Button* button);
void processButtonEvents(SDL_Event* event, Button* button);
void renderButtonText(const char* message, int x, int y, SDL_Color color);

#endif