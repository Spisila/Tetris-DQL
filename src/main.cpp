#include <raylib.h>

#include <iostream>
#include <random>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <cstdlib>

#include <vector>

#include <game.hpp>
#include <game_renderer.hpp>


int main()
{

  Game main_game{1};
  GameRenderer renderer{main_game};

  Board &board = main_game.getBoard();
  PieceQueue &queue = main_game.getPieceQueue();

  // main_game.getBoard().setPiece(PieceType::T_PIECE_0);
  // queue.generatePieceQueue();
  // queue.updatePieceQueue();

  renderer.initWindow();

  while (!WindowShouldClose())
  {
    renderer.drawLoop();
    // main_game.get_piece_queue();

    // main_game.increase_gravity_counter();

    // if (main_game.gravity_counter >= 20)
    // {
    //   std::cout << main_game.gravity_counter << std::endl;
    //   main_game.tick_gravity();
    //   main_game.gravity_counter = 0;
    // }

    if (IsKeyPressed(KEY_LEFT))
    {
      board.moveTetromino(MovementDirection::LEFT);
    }
    else if (IsKeyPressed(KEY_RIGHT))
    {
      board.moveTetromino(MovementDirection::RIGHT);
    }
    else if (IsKeyPressed(KEY_DOWN))
    {
      board.moveTetromino(MovementDirection::DOWN);
    }

    if (IsKeyPressed(KEY_Z))
    {
      board.rotateTetromino(Rotation::COUNTER_CLOCKWISE);
    }
    else if (IsKeyPressed(KEY_X))
    {
      board.rotateTetromino(Rotation::CLOCKWISE);
    }
    else if (IsKeyPressed(KEY_A))
    {
      board.rotateTetromino(Rotation::ONE_EIGHTY);
    }

    if (IsKeyPressed(KEY_C) && main_game.hold_used == false)
    {
      main_game.holdCurrentPiece();
      main_game.hold_used = true;
    }

    if (IsKeyPressed(KEY_SPACE))
    {
      main_game.hardDrop();
    }

    // int cleared_lines = main_game.clear_lines();

    // if (cleared_lines > 0)
    // {
    //   switch (cleared_lines)
    //   {
    //   case 1:
    //     main_game.score += 100;
    //     break;
    //   case 2:
    //     main_game.score += 300;
    //     break;
    //   case 3:
    //     main_game.score += 500;
    //     break;
    //   case 4:
    //     main_game.score += 1000;
    //     break;
    //   default:
    //     break;
    //   }
    // }
  }

  CloseWindow();
  return 0;
}