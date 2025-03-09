/*
Author: Ryan Wiseman
*/

//Library headers
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <sqlite3.h>
#include <iostream>

#include <array>

//App headers
#include "gameScores.h"
#include "SDLColors.h"

//Global variables
SDL_Window *window;
SDL_Renderer *renderer;

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

//Function prototypes
bool initSDL_ttf();
bool init();
void render();
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

    // Main loop for window event handling
    while (!done) 
    {
        handleEvents(done);
        render();
    }

    // Cleanup
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

bool initSDL_ttf()
{
    TTF_Init();
    font = TTF_OpenFont("assets/fonts/ArianaVioleta.ttf", 24);

    if (!font)
    {
        SDL_Log("Cannot load font!");
        return false;
    }
    
    return true;
}

bool init()
{
    SDL_Init(SDL_INIT_VIDEO);
    
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

void render()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < 3; i++)
    {
        SDL_RenderLine(
            renderer, i * SprightSize, 0, 
            i * SprightSize, ScreenHeight);
        SDL_RenderLine(
            renderer, 0, i * SprightSize, 
            ScreenWidth, i * SprightSize);
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
                SDL_RenderLine(
                    renderer, x + SprightSize - 20, 
                    y + 20, x + 20, y + SprightSize - 20);
                SDL_RenderLine(
                    renderer, x + 20, y + 20, 
                    x + SprightSize - 20, y + SprightSize - 20);
            }
            else if (board[row][col] == Player::O)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_FRect rect 
                {
                    static_cast<float>(x + 20), 
                    static_cast<float>(y + 20),
                    static_cast<float>(SprightSize - 40), 
                    static_cast<float>(SprightSize - 40)
                };
                SDL_RenderRect(renderer, &rect);
            }
        }
    }


    SDL_RenderPresent(renderer);
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
            int x = event.button.x / SprightSize;
            int y = event.button.y / SprightSize;

            if (x >= 0 && x < 3 && y >= 0 && y < 3)
            {
                if (board[y][x] == Player::NONE)
                {
                    board[y][x] = Player1;

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
                            std::cout << "Updated score for: " << 
                            winnerName << " successfully!" << std::endl;
                        }
                        else
                        {
                            std::cerr << "Failed to update score for: " 
                            << winnerName << std::endl;
                        }

                        std::cout << "Current scores in the database:" 
                        << std::endl;
                        dbManager.queryScores();
                        return;
                    }

                    if (checkWin(Player2))
                    {
                        std::string winnerName = 
                        (Player2 == Player::O) ? "Player 1" : "Player 2";
                        SDL_Log("%s wins!", winnerName.c_str());
                        SDL_Delay(1000);
                        resetBoard();

                        DatabaseManager dbManager("scoresDatabase.db");

                        if (dbManager.insertTestScore(winnerName, 1)) 
                        {
                            std::cout << "Updated score for: " << 
                            winnerName << " successfully!" << std::endl;
                        }
                        else
                        {
                            std::cerr << "Failed to update score for: " 
                            << winnerName << std::endl;
                        }

                        std::cout << "Current scores in the database:" 
                        << std::endl;
                        dbManager.queryScores();
                        return;  
                    }

                    Player1 = (Player1 == Player::X) ? 
                    Player::O : Player::X;
                }
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