#include "game.hpp"

Game::Game() {}

void Game::Init() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_UNDECORATED);
    SetExitKey(KEY_ESCAPE);
    InitWindow(640, 480, "textbasedgame");
}

void Game::Run() {
    while (!WindowShouldClose()) {
        BeginDrawing();

        DrawText(":)", 10, 10, 20, BLACK);
        DrawRectangle(50, 50, 100, 100, RED);

        EndDrawing();
    }
}