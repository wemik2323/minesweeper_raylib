#include <iostream>
#include "raylib.h"
#include <vector>

// MINEFIELD VARS
const int fieldHeight = 10;
const int fieldWidth = 10;
const int amountOfMines = 10;

// GLOB BLOB
int selectedColumn = -1;
int selectedRow = -1;
int boomCheck = 0;

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
    bool dfsCheck;
    int minesAround;
    Rectangle srcRect;

    Cell()
        : exposed{false},
          tagged{false}, mined{false}, dfsCheck {false},
          minesAround{0},
        srcRect{0, 0, 0, 0}{}
   

    bool isExposed() { return exposed; }

    bool isTagged() { return tagged; }
    void toggleTag() {
        if (tagged) {
            tagged = false;
        } else {
            tagged = true;
        }
    }

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
                srcRect = {16 * 10, 0, 16, 16};
                return;
            }

            srcRect = {float(16 * minesAround), 0, 16, 16};
        } else {
            if (isTagged()) {
                srcRect = {16 * 11, 0, 16, 16};
                return;
            } else {
                srcRect = {16 * 9, 0, 16, 16};
                return;
            }
        }
    }
};

class MineField {
   public:
    std::vector<std::vector<Cell>> matrix;

    MineField() { createField(); }

    void createField() {
        matrix.resize(fieldWidth);
        for (int i = 0; i < fieldWidth; i++) {
            for (int j = 0; j < fieldHeight; j++) {
                Cell newCell;
                matrix[i].emplace_back(newCell);
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
    }

    void resetField() { 
        matrix.clear();
        createField();
        initializeMineFiled();
        for (int i = 0; i < fieldWidth; i++) {
            for (int j = 0; j < fieldHeight; j++) {
                matrix[i][j].setSrcRect();
            }
        }
    }

    int isFieldExposed() {
        for (int i = 0; i < fieldWidth; i++) {
            for (int j = 0; j < fieldHeight; j++) {
                if (matrix[i][j].isMined()) {
                    continue;
                }
                if (!matrix[i][j].isExposed()) {
                    return 0;
                }
            }
        }
        return 1;
    }

    void openAdjacentCells(int col, int row) {
        matrix[col][row].exposed = true;
        matrix[col][row].dfsCheck = true;

        if (matrix[col][row].minesAround > 0) {
            return;
        } 

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int dx = col + i;
                int dy = row + j;
                if (i == 0 and j == 0) {
                    continue;
                }
                if ((dx < 0 or dy < 0) or  (dx >= fieldWidth or dy >= fieldHeight)) {
                    continue;
                }
                if (!matrix[dx][dy].dfsCheck) {
                    openAdjacentCells(dx, dy);
                }
            }
        }
    }


    int openCell() {
        matrix[selectedColumn][selectedRow].exposed = true;
        if (matrix[selectedColumn][selectedRow].isMined()) {
            for (int i = 0; i < fieldWidth; i++) {
                for (int j = 0; j < fieldHeight; j++) {
                    matrix[i][j].exposed = true;
                }
            }
            return -1;
        } else {
            if (matrix[selectedColumn][selectedRow].minesAround == 0) {
                openAdjacentCells(selectedColumn, selectedRow);
            }
            return 0;
        }
    }

    void drawMineFiled(Rectangle stage, Texture2D& asset, Vector2 size) {
        for (int i = 0; i < fieldWidth; i++) {
            for (int j = 0; j < fieldHeight; j++) {
                matrix[i][j].setSrcRect();
                matrix[i][j].drawCell(
                    asset, {stage.x + (i * size.x),
                            stage.y + stage.height / 6.0f +
                                (j * size.y)}, size);
            }
        }
    }
};

enum Emotions {
    CAS,
    SMILE,
    DEAD
};

class TopPanel {
   public:
    Emotions face;

    TopPanel() : face{CAS}{}

    void drawPanel(Texture2D& casFace, Texture2D& smileFace, Texture2D& deadFace, Rectangle stage) {
        switch (face) {
            case CAS:
                DrawTexturePro(
                    casFace, {0, 0, 150, 150},
                    {stage.x + stage.width / 2 - (stage.height * 4 / 36) / 2,
                     stage.y + stage.height / 36, stage.height * 4 / 36,
                     stage.height * 4 / 36},
                    {0, 0}, 0.0f, WHITE);
                break;
            case SMILE:
                DrawTexturePro(
                    smileFace, {0, 0, 150, 150},
                    {stage.x + stage.width / 2 - (stage.height * 4 / 36) / 2,
                     stage.y + stage.height / 36, stage.height * 4 / 36,
                     stage.height * 4 / 36},
                    {0, 0}, 0.0f, WHITE);
                break;
            case DEAD:
                DrawTexturePro(
                    deadFace, {0, 0, 150, 150},
                    {stage.x + stage.width / 2 - (stage.height * 4 / 36) / 2,
                     stage.y + stage.height / 36, stage.height * 4 / 36,
                     stage.height * 4 / 36},
                    {0, 0}, 0.0f, WHITE);
                break;
            default:
                break;
        }
    }
};

int CheckPannelInput(Rectangle stage, MineField& minefield, TopPanel& panel);
void CheckMouseInput(Vector2 cellSize, Rectangle stage, MineField& minefield, TopPanel& panel);

int main() {
    int width = 500;
    int height = 600;

    InitWindow(width, height, "MINEKOKER");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    Texture2D asset = LoadTexture("../resources/asset.png");

    Texture2D casFace = LoadTexture("../resources/casFace.png");
    Texture2D smileFace =
        LoadTexture("../resources/smileFace.png");
    Texture2D deadFace =
        LoadTexture("../resources/deadFace.png");
    Texture2D bg_win =
        LoadTexture("../resources/bg_texture_win.png");
    Texture2D bg_loss =
        LoadTexture("../resources/bg_texture_loss.png");


    GameScene currentScene = GAME;

    Rectangle stage;

    MineField mineField;
    mineField.initializeMineFiled();

    TopPanel panel;

    while (!WindowShouldClose()) {
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

                CheckPannelInput(stage, mineField, panel);
                CheckMouseInput(cellSize, stage, mineField, panel);

                mineField.drawMineFiled(stage, asset, cellSize);
                panel.drawPanel(casFace, smileFace, deadFace, stage);
                
                if (boomCheck == -1) {
                    panel.face = DEAD;
                    currentScene = LOSS;
                } else if (mineField.isFieldExposed() == 1) {
                    panel.face = CAS;
                    currentScene = WIN;
                }

                if (IsKeyPressed(KEY_F)) {
                    for (int i = 0; i < fieldWidth; i++) {
                        for (int j = 0; j < fieldHeight; j++) {
                            std::cout << i << " " << j << " "
                                      << mineField.matrix[i][j].isMined() << " "
                                      << mineField.matrix[i][j].isExposed()
                                      << '\n';
                        }
                    }
                }

                EndDrawing();
                break;
            case WIN:
                BeginDrawing();
                ClearBackground(RAYWHITE);

                if (IsKeyDown(KEY_SPACE)) {
                    DrawRectangle(stage.x, stage.y, stage.width, stage.height,
                                  LIGHTGRAY);
                }

                if (CheckPannelInput(stage, mineField, panel) == 1) {
                    currentScene = GAME;
                }

                if (IsKeyPressed(KEY_F)) {
                    for (int i = 0; i < fieldWidth; i++) {
                        for (int j = 0; j < fieldHeight; j++) {
                            std::cout << i << " " << j << " "
                                      << mineField.matrix[i][j].isMined() << " "
                                      << mineField.matrix[i][j].isExposed()
                                      << '\n';
                        }
                    }
                }

                DrawTexturePro(bg_win, {0, 0, 2560, 1920},
                               {stage.x, stage.y + stage.height / 6.0f,
                                stage.width, stage.height * 5.0f / 6.0f},
                               {0, 0}, 0.0f, WHITE);
                panel.drawPanel(casFace, smileFace, deadFace, stage);

                EndDrawing();
                break;
            case LOSS:
                BeginDrawing();
                ClearBackground(RAYWHITE);

                if (IsKeyDown(KEY_SPACE)) {
                    DrawRectangle(stage.x, stage.y, stage.width, stage.height,
                                  LIGHTGRAY);
                }

                if (CheckPannelInput(stage, mineField, panel) == 1) {
                    currentScene = GAME;
                    boomCheck = 0;
                }
                CheckMouseInput(cellSize, stage, mineField, panel);

                DrawTexturePro(bg_loss, {0, 0, 1620, 2160},
                               {stage.x, stage.y + stage.height / 6.0f,
                                stage.width, stage.height * 5.0f / 6.0f},
                               {0, 0}, 0.0f, WHITE);
                //mineField.drawMineFiled(stage, asset, cellSize);
                panel.drawPanel(casFace,smileFace,deadFace,stage);

                EndDrawing();
                break;
            default:
                currentScene = MENU;
                break;
        }
    }

    UnloadTexture(asset);
    UnloadTexture(casFace);
    UnloadTexture(smileFace);
    UnloadTexture(deadFace);
    UnloadTexture(bg_loss);
    UnloadTexture(bg_win);

    return 0;
}

int CheckPannelInput(Rectangle stage, MineField& minefield, TopPanel& panel) {
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(
                mousePos,
                {stage.x + stage.width / 2 - (stage.height * 4 / 36) / 2,
                 stage.y + stage.height / 36, stage.height * 4 / 36,
                 stage.height * 4 / 36})) {
            minefield.resetField();
            panel.face = CAS;
            boomCheck = 0;
            return 1;
        }
    }
    return -1;
}

void CheckMouseInput(Vector2 cellSize, Rectangle stage, MineField& minefield, TopPanel& panel) {
    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

        for (int i = 0; i < fieldWidth; i++) {
            for (int j = 0; j < fieldHeight; j++) {
                if (minefield.matrix[i][j].isExposed() or
                    minefield.matrix[i][j].isTagged()) {
                    continue;
                }
                Rectangle cellRect{
                    stage.x + (i * cellSize.x),
                    stage.y + stage.height / 6.0f + (j * cellSize.y), cellSize.x, cellSize.y};
                if (CheckCollisionPointRec(mousePos, cellRect)) {
                    selectedColumn = i;
                    selectedRow = j;
                    return;
                }
            }
        }
    } 
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        for (int i = 0; i < fieldWidth; i++) {
            for (int j = 0; j < fieldHeight; j++) {
                Rectangle cellRect{stage.x + (i * cellSize.x),
                            stage.y + stage.height / 6.0f +
                                (j * cellSize.y), cellSize.x, cellSize.y};
                if (CheckCollisionPointRec(mousePos, cellRect)) {
                    minefield.matrix[i][j].toggleTag();
                    return;
                }
            }
        }
    }
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) and selectedColumn != -1 and selectedRow != -1) {
            panel.face = SMILE;
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) and selectedColumn != -1 and selectedRow != -1) {       
        Rectangle cellRect{stage.x + (selectedColumn * cellSize.x),
                           stage.y + stage.height / 6.0f + (selectedRow * cellSize.y),
                           cellSize.x, cellSize.y};
        if (CheckCollisionPointRec(mousePos, cellRect)) {
            boomCheck = minefield.openCell();
        }

        selectedColumn = -1;
        selectedRow = -1;
        panel.face = CAS;
    }
}