#include "screenScenes.h"

void handleMainMenu(SDL_Renderer* renderer, SceneState& currentScene)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.scancode == SDLK_SPACE) {
                currentScene = SceneState::GAME;
            }
        }
    }
}

void handleGame(SDL_Renderer* renderer, SceneState& currentScene)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.scancode == SDLK_ESCAPE) {
                currentScene = SceneState::END_SCREEN;
            }
        }
    }
}

void handleEndScreen(SDL_Renderer* renderer, SceneState& currentScene)
{
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.scancode == SDLK_R) {
                currentScene = SceneState::MAIN_MENU;
            }
        }
    }
}

