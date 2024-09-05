#include <iostream>
#include "raylib.h"

enum GameScene {
	MENU,
	GAME,
	WIN,
	LOSS
};

int main() {
    int width = 500;
    int height = 800;

	InitWindow(width, height, "MINEKOKER");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

	GameScene currentScene = MENU;

	while (!WindowShouldClose()) {

        width = GetScreenWidth();
        height = GetScreenHeight();
        
        Rectangle stage;

        if (height < width / 5 * 8) {
            stage.height = GetScreenHeight();
            stage.width = stage.height / 8 * 5;
        } else {
            stage.width = GetScreenWidth();
            stage.height = stage.width / 5 * 8;
        }

        stage.x = GetScreenWidth() / 2 - stage.width / 2;
        stage.y = GetScreenHeight() / 2 - stage.height / 2;

        switch (currentScene) {
            case MENU:
                BeginDrawing();
                ClearBackground(RAYWHITE);

                if (IsKeyPressed(KEY_ENTER)) {
                    currentScene = GAME;
                }

                DrawText("PRESS ENTERINO", stage.x + stage.width/4,
                         stage.y + stage.height/4, 40, RED);
                EndDrawing();
                break;
            case GAME:
                BeginDrawing();
                ClearBackground(RAYWHITE);

                if (IsKeyDown(KEY_SPACE)) {
                    DrawRectangle(stage.x, stage.y, stage.width, stage.height, LIGHTGRAY);
                }

                EndDrawing();
                break;
            case WIN:
                BeginDrawing();
                ClearBackground(RAYWHITE);

                EndDrawing();
                break;
            case LOSS:
                BeginDrawing();
                ClearBackground(RAYWHITE);

                EndDrawing();
                break;
            default:
                currentScene = MENU;
                break;
        }
    }

	return 0;
}