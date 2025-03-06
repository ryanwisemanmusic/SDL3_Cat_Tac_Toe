/*
Author: Ryan Wiseman

This is a barebones approach to windowing via SDL3. Any
intenseive windowing required will require some major refactoring
*/
#define SDL_MAIN_HANDLED  // Required, not allowed to change

#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <array>
#include <string>
#include <SDL3/SDL_surface.h>

// Global variables
SDL_Window* gWindow{ nullptr };
SDL_Renderer* gRenderer{ nullptr };

// Constexpr integers
constexpr int kScreenWidth{ 600 };
constexpr int kScreenHeight{ 600 };
constexpr int kCellSize{ 200 };

enum class Player { NONE, X, O };
std::array<std::array<Player, 3>, 3> board{};

Player currentPlayer = Player::X;

// Function prototypes
bool init();
void render();
void handleEvents(bool& done);
bool loadMedia();  // Unused; consider implementing or removing.
void close();
bool checkWin(Player player);
void resetBoard();

// Cocoa functions
extern "C" void cocoaBaseMenuBar();
extern "C" void openSDLWindowAboutMenu();

int main() {
    // Move Cocoa initialization before SDL_Init
    cocoaBaseMenuBar();

    if (!init()) {
        SDL_Log("Unable to initialize program!\n");
        return 1;
    }

    bool done = false;
    while (!done) {
        handleEvents(done);
        render();
        // Optional: SDL_Delay(1); // Minimal delay to avoid hogging the CPU.
    }

    close();
    return 0;
}

bool init() {
    // Initialize SDL video
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return false;
    }

    // Create window with SDL_WINDOW_OPENGL flag (required in SDL3)
    gWindow = SDL_CreateWindow("SDL3 Tic-Tac-Toe", kScreenWidth, kScreenHeight, SDL_WINDOW_OPENGL);
    if (!gWindow) {
        SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    SDL_SetWindowPosition(gWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    // Create renderer with no extra flags (avoid SDL_RENDERER_ACCELERATION)
    gRenderer = SDL_CreateRenderer(gWindow, 0);
    if (!gRenderer) {
        SDL_Log("Renderer could not be created! SDL error: %s\n", SDL_GetError());
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        return false;
    }

    // Enable VSync manually in SDL3
    if (SDL_SetRenderVSync(gRenderer, true) != 0) {
        SDL_Log("Failed to enable VSync! SDL Error: %s\n", SDL_GetError());
    }

    return true;
}

void render() {
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
    SDL_RenderClear(gRenderer);

    // Draw grid lines
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    for (int i = 1; i < 3; ++i) {
        SDL_RenderLine(gRenderer, i * kCellSize, 0, i * kCellSize, kScreenHeight);
        SDL_RenderLine(gRenderer, 0, i * kCellSize, kScreenWidth, i * kCellSize);
    }

    // Draw Xs and Os
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            int x = col * kCellSize;
            int y = row * kCellSize;

            if (board[row][col] == Player::X) {
                SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);  // Red for X
                SDL_RenderLine(gRenderer, x + 20, y + 20, x + kCellSize - 20, y + kCellSize - 20);
                SDL_RenderLine(gRenderer, x + kCellSize - 20, y + 20, x + 20, y + kCellSize - 20);
            } else if (board[row][col] == Player::O) {
                SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);  // Blue for O
                SDL_FRect rect{ static_cast<float>(x + 20), static_cast<float>(y + 20), 
                                static_cast<float>(kCellSize - 40), static_cast<float>(kCellSize - 40) };
                SDL_RenderRect(gRenderer, &rect);
            }
        }
    }

    SDL_RenderPresent(gRenderer);
}

void handleEvents(bool& done) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            done = true;
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            int x = event.button.x / kCellSize;
            int y = event.button.y / kCellSize;

            // Bounds checking to avoid out-of-range access.
            if (x >= 0 && x < 3 && y >= 0 && y < 3) {
                // If the cell is empty, place current player's mark
                if (board[y][x] == Player::NONE) {
                    board[y][x] = currentPlayer;

                    // Check if current player wins
                    if (checkWin(currentPlayer)) {
                        SDL_Log("%s wins!", (currentPlayer == Player::X) ? "X" : "O");
                        SDL_Delay(1000);
                        resetBoard();
                    } else {
                        currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
                    }
                }
            }
        }
    }
}

bool checkWin(Player player) {
    // Check rows, columns, and diagonals
    for (int i = 0; i < 3; i++) {
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
            (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
            return true;
        }
    }
    return (board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
           (board[0][2] == player && board[1][1] == player && board[2][0] == player);
}

void resetBoard() {
    for (auto& row : board) {
        row.fill(Player::NONE);
    }
    currentPlayer = Player::X;
}

void close() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
}
