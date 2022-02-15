#include "game.hpp"

Game::Game() {

    // textbox read is bound to tbg eval
    // tbg write is bound to textbox set text

    textbox = TextBox( [&](std::string s){ tbg.EvalPlayerInput(s); } );
    tbg = TextBasedGame( [&](std::string s){ textbox.Write(s); } );
}

Game::~Game() {}

void Game::Init() {
    // do not make it resizable
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_UNDECORATED);
    InitWindow(Game::WinWidth, Game::WinHeight, "textbasedgame");

    roomImgTexture = LoadTexture("./assets/a.png");
}

void Game::Run() {
    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);
        DrawTexture(roomImgTexture, 0, 0, WHITE);

        textbox.Draw();
        EndDrawing();
    }
}
