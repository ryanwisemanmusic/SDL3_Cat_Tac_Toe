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
// bool loadMedia();  // Unused; consider implementing or removing.
void close();
// bool checkWin(Player player);  // Commented out to avoid undefined symbols
// void resetBoard();  // Commented out to avoid undefined symbols

// Cocoa functions
extern "C" void cocoaBaseMenuBar();
// extern "C" void openSDLWindowAboutMenu();  // Commented out for now

int main() {
    // First, initialize SDL and windowing.
    if (!init()) {  
        SDL_Log("Unable to initialize program!\n");
        return 1;
    }

    // Once SDL_Init() and window creation succeed, set up Cocoa.
    cocoaBaseMenuBar();

    bool done = false;
    while (!done) {
        handleEvents(done);  // This will not be called due to being commented out
        render();
    }

    close();
    return 0;
}

bool init() {
    // Initialize SDL video subsystem.
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }
    SDL_Log("SDL_Init succeeded.");

    // Create window without any additional flags.
    gWindow = SDL_CreateWindow("SDL3 Tic-Tac-Toe", kScreenWidth, kScreenHeight, 0);
    if (!gWindow) {
        SDL_Log("SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    SDL_Log("Window created successfully.");
    SDL_SetWindowPosition(gWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    // Create renderer using SDL3 properties.
    SDL_PropertiesID rendererProps = SDL_CreateProperties();
    if (!rendererProps) {
        SDL_Log("SDL_CreateProperties failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        return false;
    }
    // Tell the renderer which window to create for.
    SDL_SetPointerProperty(rendererProps, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, gWindow);
    gRenderer = SDL_CreateRendererWithProperties(rendererProps);
    SDL_DestroyProperties(rendererProps);
    if (!gRenderer) {
        SDL_Log("SDL_CreateRendererWithProperties failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        return false;
    }
    SDL_Log("Renderer created successfully.");

    // Enable VSync.
    if (SDL_SetRenderVSync(gRenderer, true) != 0) {
        SDL_Log("Failed to enable VSync: %s\n", SDL_GetError());
    } else {
        SDL_Log("VSync enabled successfully.");
    }

    return true;
}

void render() {
    // Clear screen to white.
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
    SDL_RenderClear(gRenderer);

    // Draw grid lines in black.
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    for (int i = 1; i < 3; ++i) {
        SDL_RenderLine(gRenderer, i * kCellSize, 0, i * kCellSize, kScreenHeight);
        SDL_RenderLine(gRenderer, 0, i * kCellSize, kScreenWidth, i * kCellSize);
    }

    // Draw Xs and Os.
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
    /* Optional: Event handling and game logic for later testing.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            done = true;
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            int x = event.button.x / kCellSize;
            int y = event.button.y / kCellSize;

            if (x >= 0 && x < 3 && y >= 0 && y < 3) {
                if (board[y][x] == Player::NONE) {
                    board[y][x] = currentPlayer;
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
    */
}

bool checkWin(Player player) {
    /* Optional: Game win checking logic for later testing
    for (int i = 0; i < 3; i++) {
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
            (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
            return true;
        }
    }
    return (board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
           (board[0][2] == player && board[1][1] == player && board[2][0] == player);
    */
}

void resetBoard() {
    /* Optional: Reset board logic for later testing
    for (auto& row : board) {
        row.fill(Player::NONE);
    }
    currentPlayer = Player::X;
    */
}

void close() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
}
