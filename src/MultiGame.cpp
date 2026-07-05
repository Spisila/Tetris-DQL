
#include <iostream>
#include <pybind11/pybind11.h>

namespace py = pybind11;

#include <MultiGame.hpp>

MultiGame::MultiGame(size_t parallel_game_count)
    : barrier(std::thread::hardware_concurrency())
{

  GAME_AMOUNT = parallel_game_count;

  running = true;

  unsigned int num_cores = std::thread::hardware_concurrency();

  for (int i = 0; i < GAME_AMOUNT; i++)
  {
    games.emplace_back(i);
    states_cache.push_back(games[i].getGameState());
  }

  int segment_size = GAME_AMOUNT / num_cores;

  std::span<Game> game_view{games};

  for (size_t i = 0; i < num_cores; i++)
  {

    if (i == num_cores - 1)
    {
      segments.push_back(game_view.subspan(i * segment_size));
      continue;
    }

    segments.push_back(game_view.subspan(i * segment_size, segment_size));

    game_workers.push_back(std::thread(&MultiGame::threadLoop, this, i));
  }
}

MultiGame::~MultiGame()
{

  barrier.arrive_and_wait();
  running = false;

  for (auto &worker : game_workers)
  {
    if (worker.joinable())
    {
      worker.join();
    }
  }
}

void MultiGame::threadLoop(size_t thread_segment_index)
{

  while (running)
  {
    barrier.arrive_and_wait();

    if (!running)
    {
      break;
    }

    auto thread_segment = segments[thread_segment_index];

    for (auto &game : thread_segment)
    {
      game.step();
    }

    barrier.arrive_and_wait();
  }
}

std::vector<StepData> MultiGame::stepAll(std::vector<Actions> _actions)
{

  py::gil_scoped_release release;

  for (int i = 0; i < games.size(); i++)
  {
    games[i].next_action = _actions[i];
  }

  barrier.arrive_and_wait();

  barrier.arrive_and_wait();

  std::vector<StepData> rewards;

  for (int i = 0; i < games.size(); i++)
  {
    rewards.push_back(games[i].calculateReward());
    states_cache[games[i].getId()] = games[i].getGameState();
  }

  return rewards;
}

void MultiGame::stepThis(int index, Actions _action)
{
  games[index].step();
}

void MultiGame::resetAll()
{

  for (int i = 0; i < games.size(); i++)
  {
    games[i].reset();
  }
}

void MultiGame::resetThis(int index)
{

  games[index].reset();
}

// int MultiGame::getSumScore()
// {
//   int sum = 0;
//   for (Game &game : games)
//   {
//     sum += game.get_score();
//   }
//   return sum;
// }

int MultiGame::getLinesCleared()
{
  int sum = 0;
  for (Game &game : games)
  {
    sum += game.lines_cleared;
  }
  return sum;
}

std::vector<std::vector<int>> MultiGame::getStates()
{
  return states_cache;
}

void MultiGame::initGraphics()
{
    SetTargetFPS(60);

    SetTraceLogLevel(LOG_WARNING);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Test");
}

void MultiGame::render(std::string &generation_counter)
{
    PollInputEvents();

    if (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        DrawText(TextFormat("Generation = %s", generation_counter), SCREEN_WIDTH / 2 + 400, SCREEN_HEIGHT / 2 - 400, 30, RED);

        auto board_cells = games[0].getBoard().getBoardCells();
        Tetromino t = games[0].getBoard().getBoardTetromino();

        for (int x = 0; x < board_cells.size(); x++)
        {
            for (int y = 0; y < board_cells[x].size(); y++)
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

                if (board_cells[x][y] == CellState::FILLED)
                {
                    cell_color = FILLED_COLOR;
                }

                if (board_cells[x][y] == CellState::ACTIVE)
                {
                    int active_piece = static_cast<int>(t.getCurrentPieceType());

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

        EndDrawing();
    }
}

void MultiGame::closeGraphics()
{
    CloseWindow();
}