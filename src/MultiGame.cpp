
#include <iostream>
#include <pybind11/pybind11.h>

namespace py = pybind11;

#include <MultiGame.hpp>

MultiGame::MultiGame(size_t parallel_game_count)
    : barrier(std::thread::hardware_concurrency())
{

  GAME_AMOUNT = parallel_game_count;

  // states_cache.resize(GAME_AMOUNT);

  running = true;

  unsigned int num_cores = std::thread::hardware_concurrency();

  for (int i = 0; i < GAME_AMOUNT; i++)
  {
    games.emplace_back(i);
    states_cache.push_back(games[i].get_game_state());
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
    rewards.push_back(games[i].calculate_reward());
    states_cache[games[i].get_id()] = games[i].get_game_state();
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

int MultiGame::getSumScore()
{
  int sum = 0;
  for (Game &game : games)
  {
    sum += game.get_score();
  }
  return sum;
}

std::vector<std::vector<int>> MultiGame::getStates()
{

  // py::gil_scoped_release release;

  // std::vector<std::future<std::vector<int>>> futures;

  // for (int i = 0; i < segments.size(); i++)
  // {

  //   Game &game_i = games[i];

  //   futures.push_back(std::async(std::launch::async, &Game::get_game_state, &game_i));
  // }

  // std::vector<std::vector<int>> states;

  // for (auto &future : futures)
  // {
  //   states.push_back(future.get());
  // }

  return states_cache;
}
