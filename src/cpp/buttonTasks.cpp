#include <iostream>
#include <string>
#include <cstdint>

#include "buttonTasks.h"
extern TTF_Font* font;

void drawButton(SDL_Renderer *renderer, const Button *button) 
{
    SDL_Color prevColor;
    SDL_GetRenderDrawColor(renderer, &prevColor.r, &prevColor.g, &prevColor.b, &prevColor.a);

    SDL_FRect rect = 
    { 
        static_cast<float>(button->rect.x), 
        static_cast<float>(button->rect.y), 
        static_cast<float>(button->rect.w), 
        static_cast<float>(button->rect.h) 
    };
    SDL_SetRenderDrawColor(renderer, button->color.r, 
        button->color.g, button->color.b, button->color.a);
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, prevColor.r, prevColor.g, prevColor.b, prevColor.a);
}

void processButtonEvents(SDL_Event *event, Button *button)
{
    if (event->type == 
        SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button 
        == SDL_BUTTON_LEFT) 
    {
        int x = event->button.x;
        int y = event->button.y;
        if (x >= button->rect.x && x < button->rect.x + button->rect.w &&
            y >= button->rect.y && y < button->rect.y + button->rect.h) 
        {
            button->pressed = true;
            if (button->callback) 
            {
                button->callback(button->userdata);
            }
        }
    }
    if (event->type == 
        SDL_EVENT_MOUSE_BUTTON_UP && event->button.button == 
        SDL_BUTTON_LEFT) 
    {
        button->pressed = false;
    }
}


void renderButtonText(
    const char* message, int x, int y, SDL_Color color)
{
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
