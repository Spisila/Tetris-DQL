#pragma once

#include <raylib.h>
#include <game.hpp>

constexpr Color SPAWN_COLOR = Color{75, 75, 75, 255};
constexpr Color BOARD_COLOR = Color{15, 15, 15, 255};
constexpr Color FILLED_COLOR = Color{100, 100, 100, 255};

constexpr int SCREEN_WIDTH = 1800;
constexpr int SCREEN_HEIGHT = 900;
constexpr int CELL_SIZE = 30;

constexpr Color BACKGROUND_COLOR = Color{25, 25, 75, 255};

constexpr int OFFSET_X = SCREEN_WIDTH / 2 - 250;
constexpr int OFFSET_Y = SCREEN_HEIGHT / 2 - 300;

class GameRenderer
{

private:
  Game &game;

public:
  GameRenderer(Game &game);

  void initWindow();

  void drawLoop();

  void drawBoard(const Game &game);

  char pieceTypeToChar(PieceType piece);
};