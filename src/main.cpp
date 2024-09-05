#include <iostream>
#include "raylib.h"
#include <vector>
#include <print>

// MINEFIELD VARS
const int fieldHeight = 10;
const int fieldWidth = 10;
const int amountOfMines = 30;

enum GameScene {
	MENU,
	GAME,
	WIN,
	LOSS
};

class Cell {
   public:
    bool exposed;
    bool tagged;
    bool mined;
    int minesAround;
    Rectangle srcRect;

    Cell()
        : exposed{false},
          tagged{false},
          mined{false},
          minesAround{0},
        srcRect{0, 0, 0, 0}{}
   

    bool isExposed() { return exposed; }

    bool isTagged() { return tagged; }

    bool isMined() { return mined; }
    void setMine() { mined = true; }
    
    void plusMineAround() { minesAround++; }

    void drawCell(Texture2D& asset, Vector2 position, Vector2 size) {
        DrawTexturePro(asset, srcRect,
                       {position.x, position.y, size.x, size.y}, { 0, 0}, 0.0f, WHITE);
    }

    void setSrcRect() {
        if (isExposed()) {
            if (isMined()) {
                srcRect = {16 * 10, 16};
                return;
            }

            int texCol = minesAround - 1;
            srcRect = {float(16 * texCol), 16};
        } else {
            if (isTagged()) {
                srcRect = {16 * 11, 16};
                return;
            } else {
                srcRect = {16 * 9, 16};
                return;
            }
        }
    }
};

class MineField {
   public:
    std::vector<std::vector<Cell>> matrix;

    MineField() {
        matrix.resize(fieldWidth);
        for (int i = 0; i < fieldWidth; i++) {
            matrix[i].resize(fieldHeight);
            for (int j = 0; j < fieldHeight; j++) {
                Cell newCell;
                matrix[i].push_back(newCell);
            }
        }
    }

    void initializeMineFiled() {
        int placedMines = 0;
        while (placedMines < amountOfMines) {
            int row = rand() % fieldHeight;
            int col = rand() % fieldWidth;

            if (matrix[row][col].isMined()) continue;

            matrix[row][col].setMine();
            placedMines++;

            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int newRow = row + i;
                    int newCol = col + j;

                    if (newRow >= 0 && newRow < fieldHeight && newCol >= 0 &&
                        newCol < fieldWidth &&
                        !matrix[newRow][newCol].isMined()) {
                        matrix[newRow][newCol].plusMineAround();
                    }
                }
            }
        }
        for (int i = 0; i < matrix.size(); i++) {
            for (int j = 0; j < matrix[i].size(); j++) {
                matrix[i][j].setSrcRect();
            }
        }
    }

    void drawMineFiled(Rectangle stage, Texture2D& asset, Vector2 size) {
        for (int i = 0; i < matrix.size(); i++) {
            for (int j = 0; j < matrix[i].size(); j++) {
                matrix[i][j].drawCell(
                    asset, {stage.x + (i * size.x),
                            stage.y + stage.height / 6.0f +
                                (j * size.y)}, size);
            }
        }
    }
};


int main() {
    int width = 500;
    int height = 600;

    InitWindow(width, height, "MINEKOKER");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    Texture2D asset = LoadTextureFromImage(LoadImage("../resources/asset.png"));

    GameScene currentScene = MENU;

    while (!WindowShouldClose()) {

        Rectangle stage;
        width = GetScreenWidth();
        height = GetScreenHeight();
        if (height < float(width) / 5.0f * 6.0f) {
            stage.height = GetScreenHeight();
            stage.width = stage.height / 6.0 * 5.0;
        } else {
            stage.width = GetScreenWidth();
            stage.height = stage.width / 5.0 * 6.0;
        }
        stage.x = GetScreenWidth() / 2.0 - stage.width / 2.0;
        stage.y = GetScreenHeight() / 2.0 - stage.height / 2.0;
        Vector2 cellSize {stage.width / float(fieldWidth),
                         5.0f / 6.0f * stage.height / fieldHeight};

        MineField mineField;
        mineField.initializeMineFiled();

        switch (currentScene) {
            case MENU:
                BeginDrawing();
                ClearBackground(RAYWHITE);

                if (IsKeyPressed(KEY_ENTER)) {
                    currentScene = GAME;
                }

                DrawText("PRESS ENTERINO", stage.x + stage.width / 4,
                            stage.y + stage.height / 4, 40, RED);
                EndDrawing();
                break;
            case GAME:
                BeginDrawing();
                ClearBackground(RAYWHITE);

                if (IsKeyDown(KEY_SPACE)) {
                    DrawRectangle(stage.x, stage.y, stage.width,
                                    stage.height, LIGHTGRAY);
                }

                DrawTexture(asset, stage.x, stage.y, WHITE);
                mineField.drawMineFiled(stage, asset, cellSize);

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