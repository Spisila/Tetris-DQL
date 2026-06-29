#include <raylib.h>

#include <iostream>
#include <random>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <cstdlib>

#include <vector>

#include <game.hpp>

void draw_board(const Game &game)
{
  for (int x = 0; x < game.board.size(); x++)
  {
    for (int y = 0; y < game.board.at(x).size(); y++)
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

      if (game.board.at(x).at(y) == Cell_state::FILLED)
      {
        cell_color = FILLED_COLOR;
      }

      if (game.board.at(x).at(y) == Cell_state::ACTIVE)
      {
        int active_piece = static_cast<int>(game.active_tetromino.current_type);

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

char piece_type_to_char(Piece_type piece)
{
  switch (piece)
  {
  case Piece_type::I_PIECE_0:
    return 'I';
    break;

  case Piece_type::J_PIECE_0:

    return 'J';
    break;

  case Piece_type::L_PIECE_0:
    return 'L';
    break;

  case Piece_type::O_PIECE_0:

    return 'O';
    break;

  case Piece_type::S_PIECE_0:

    return 'S';
    break;

  case Piece_type::T_PIECE_0:

    return 'T';
    break;

  case Piece_type::Z_PIECE_0:

    return 'Z';
    break;

  default:
    return '?';
    break;
  }
}

int main()
{

  Game main_game(1);

  SetTargetFPS(60);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GEORGE TETRIS");

  main_game.set_piece(Piece_type::T_PIECE_0);
  main_game.generate_piece_queue();
  main_game.update_piece_queue();

  while (!WindowShouldClose())
  {
    // main_game.get_piece_queue();

    main_game.increase_gravity_counter();

    if (main_game.gravity_counter >= 20)
    {
      std::cout << main_game.gravity_counter << std::endl;
      main_game.tick_gravity();
      main_game.gravity_counter = 0;
    }

#pragma region Inputs

    if (IsKeyPressed(KEY_LEFT))
    {
      main_game.move_tetromino(Movement_direction::LEFT);
    }
    else if (IsKeyPressed(KEY_RIGHT))
    {
      main_game.move_tetromino(Movement_direction::RIGHT);
    }
    else if (IsKeyPressed(KEY_DOWN))
    {
      main_game.move_tetromino(Movement_direction::DOWN);
    }

    if (IsKeyPressed(KEY_Z))
    {
      main_game.rotate_tetromino(Rotation::COUNTER_CLOCKWISE);
    }
    else if (IsKeyPressed(KEY_X))
    {
      main_game.rotate_tetromino(Rotation::CLOCKWISE);
    }
    else if (IsKeyPressed(KEY_A))
    {
      main_game.rotate_tetromino(Rotation::ONE_EIGHTY);
    }

    if (IsKeyPressed(KEY_C) && main_game.hold_used == false)
    {
      main_game.hold_current_piece();
      main_game.hold_used = true;
    }

    if (IsKeyPressed(KEY_SPACE))
    {
      main_game.hard_drop();
    }

#pragma endregion

    int cleared_lines = main_game.clear_lines();

    if (cleared_lines > 0)
    {
      switch (cleared_lines)
      {
      case 1:
        main_game.score += 100;
        break;
      case 2:
        main_game.score += 300;
        break;
      case 3:
        main_game.score += 500;
        break;
      case 4:
        main_game.score += 1000;
        break;
      default:
        break;
      }
    }

    BeginDrawing();

    ClearBackground(BACKGROUND_COLOR);

    draw_board(main_game);

    DrawText(TextFormat("QUEUE = %d", main_game.queue_index), SCREEN_WIDTH / 2 + 400, SCREEN_HEIGHT / 2 - 400, 30, RED);
    DrawText(TextFormat("CURRENT = %c", piece_type_to_char(main_game.active_tetromino.current_type)), SCREEN_WIDTH / 2 + 400, SCREEN_HEIGHT / 2 - 360, 30, RED);
    DrawText(TextFormat("QUEUE = %c %c %c %c %c", piece_type_to_char(main_game.known_piece_queue.at(0)),
                        piece_type_to_char(main_game.known_piece_queue.at(1)),
                        piece_type_to_char(main_game.known_piece_queue.at(2)),
                        piece_type_to_char(main_game.known_piece_queue.at(3)),
                        piece_type_to_char(main_game.known_piece_queue.at(4))),
             SCREEN_WIDTH / 2 + 400, SCREEN_HEIGHT / 2 - 320, 30, RED);

    DrawText(TextFormat("HOLD = %c", piece_type_to_char(static_cast<Piece_type>(main_game.hold_piece_index))), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT / 2 - 400, 30, RED);

    DrawText(TextFormat("SCORE = %d", main_game.score), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT / 2 - 375, 30, WHITE);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}