#include <raylib.h>

#include <iostream>
#include <random>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <cstdlib>

#include <vector>

#include <game.hpp>

void drawBoard(const Game &game)
{

  const auto &board = game.getBoard();
  auto cells = board.getBoardCells();

  for (int x = 0; x < cells.size(); x++)
  {
    for (int y = 0; y < cells[x].size(); y++)
    {
      Color cell_color;

      if (y < 4)
      {
        cell_color = SPAWN_COLOR;
      }
      else
      {
        cell_color = BOARD_COLOR;
      }

      if (cells[x][y] == CellState::FILLED)
      {
        cell_color = FILLED_COLOR;
      }

      if (cells[x][y] == CellState::ACTIVE)
      {
        int active_piece = static_cast<int>(board.getBoardTetromino().getCurrentPieceType());

        if (active_piece < 4)
          cell_color = SKYBLUE;
        else if (active_piece >= 4 && active_piece < 8)
          cell_color = DARKBLUE;
        else if (active_piece >= 8 && active_piece < 12)
          cell_color = ORANGE;
        else if (active_piece >= 12 && active_piece < 16)
          cell_color = YELLOW;
        else if (active_piece >= 16 && active_piece < 20)
          cell_color = GREEN;
        else if (active_piece >= 20 && active_piece < 24)
          cell_color = PURPLE;
        else if (active_piece >= 24 && active_piece < 28)
          cell_color = RED;
      }

      DrawRectangle(OFFSET_X + x * CELL_SIZE, OFFSET_Y + y * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, cell_color);
    }
  }
}

char pieceTypeToChar(PieceType piece)
{
  switch (piece)
  {
  case PieceType::I_PIECE_0:
    return 'I';
    break;

  case PieceType::J_PIECE_0:

    return 'J';
    break;

  case PieceType::L_PIECE_0:
    return 'L';
    break;

  case PieceType::O_PIECE_0:

    return 'O';
    break;

  case PieceType::S_PIECE_0:

    return 'S';
    break;

  case PieceType::T_PIECE_0:

    return 'T';
    break;

  case PieceType::Z_PIECE_0:

    return 'Z';
    break;

  default:
    return '?';
    break;
  }
}

int main()
{

  Game main_game{1};

  Board &board = main_game.getBoard();
  PieceQueue &queue = main_game.getPieceQueue();

  SetTargetFPS(60);

  SetTraceLogLevel(LOG_WARNING);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GEORGE TETRIS");

  // main_game.getBoard().setPiece(PieceType::T_PIECE_0);
  // queue.generatePieceQueue();
  // queue.updatePieceQueue();

  while (!WindowShouldClose())
  {
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

    BeginDrawing();

    ClearBackground(BACKGROUND_COLOR);

    drawBoard(main_game);

    // DrawText(TextFormat("QUEUE = %d", main_game.queue_index), SCREEN_WIDTH / 2 + 400, SCREEN_HEIGHT / 2 - 400, 30, RED);
    // DrawText(TextFormat("CURRENT = %c", pieceTypeToChar(static_cast<PieceType>(main_game.getBoard().getBoardTetromino().getCurrentPieceType()))), SCREEN_WIDTH / 2 + 400, SCREEN_HEIGHT / 2 - 360, 30, RED);
    // DrawText(TextFormat("QUEUE = %c %c %c %c %c",
    //                     pieceTypeToChar(static_cast<PieceType>(main_game.getPieceQueue().getPieceQueue().at(0))),
    //                     pieceTypeToChar(static_cast<PieceType>(main_game.getPieceQueue().getPieceQueue().at(1))),
    //                     pieceTypeToChar(static_cast<PieceType>(main_game.getPieceQueue().getPieceQueue().at(2))),
    //                     pieceTypeToChar(static_cast<PieceType>(main_game.getPieceQueue().getPieceQueue().at(3))),
    //                     pieceTypeToChar(static_cast<PieceType>(main_game.getPieceQueue().getPieceQueue().at(4)))),
    //          SCREEN_WIDTH / 2 + 400, SCREEN_HEIGHT / 2 - 320, 30, RED);

    DrawText(TextFormat("HOLD = %c", pieceTypeToChar(static_cast<PieceType>(main_game.hold_piece_index))), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT / 2 - 400, 30, RED);

    // DrawText(TextFormat("SCORE = %d", main_game.score), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT / 2 - 375, 30, WHITE);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}