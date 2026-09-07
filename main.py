
import os
import sys
import time

import numpy as np

import random

import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F

from python.DQN            import DQN
from python.replay_buffer  import ReplayBuffer
from python.episode_logger import EpisodeLogger
from python.episode_watcher import EpisodeWatcher

agent_path = os.path.abspath("./Release")
sys.path.append(agent_path)

from Debug import Tetris_AGENT

        

def sigmoid_scale(x, k=5.0):
    return (2.0 / (1.0 + np.exp(-x / k))) - 1.0

def get_all_current_state(games: Tetris_AGENT.MultiGame) :
    
    state_data = games.getStates()

    scaled_states = []

    for i in range(len(state_data)) :

        scaled_height = sigmoid_scale(state_data[i][0], 30)
        scaled_holes = sigmoid_scale(state_data[i][1], 5)
        scaled_rugosity = sigmoid_scale(state_data[i][2], 10)
        scaled_current_piece = state_data[i][3] / 6
        
        queue_tensor = torch.tensor(state_data[i][4:])
        hot_one_piece_queue = F.one_hot(queue_tensor, num_classes=7)

        flat_hot_one = hot_one_piece_queue.flatten()

        concat = np.concatenate([[scaled_height], [scaled_holes], [scaled_rugosity], [scaled_current_piece], flat_hot_one])

        scaled_states.append(concat)

    return scaled_states

def print_current_state(estado):
    
    agg_height = estado[0]
    holes      = estado[1]
    rugosity   = estado[2]
    piece      = int(estado[3])
    
    queue      = [int(p) for p in estado[4:]]
    
    print(f"--- State ---")
    print(f"AGG height:         {agg_height:.4f}")
    print(f"Holes:              {holes:.4f}")
    print(f"Rugosity:           {rugosity:.4f}")
    print(f"Current piece:      {piece}")
    print(f"Queue:              {queue}")

def back_propagation(samples) :

    base_qs = []

    bellman_results = []

    old_states_matrix =  np.array([sample[0] for sample in samples])
    old_actions_matrix = np.array([sample[1] for sample in samples])
    old_rewards_matrix = np.array([sample[2] for sample in samples])
    new_states_matrix =  np.array([sample[3] for sample in samples])

    old_states_tenson = torch.from_numpy(old_states_matrix)
    old_actions_tensor = torch.from_numpy(old_actions_matrix)
    old_rewards_tensor = torch.from_numpy(old_rewards_matrix)
    new_states_tensor = torch.from_numpy(new_states_matrix)

    q_values = model(old_states_tenson)
    base_qs =  q_values.gather(1, old_actions_tensor.unsqueeze(-1)).squeeze(-1)

    target_q_values = target_model(new_states_tensor)

    max_target_qs = torch.max(target_q_values, dim=1)[0]

    bellman_results = old_rewards_tensor + (0.99 * max_target_qs)

    optimizer.zero_grad()

    loss = criterion(base_qs, bellman_results)

    loss.backward()

    optimizer.step()

print("Started")


NUM_NEURONS = 128
NUM_GAMES   = 8
SET_TARGET_IN_ACTIONS = 500

LOG_TRAINING_IN_EPISODES = 100

WATCH_TRANING = True

INPUT_DIMENSIONS = 39
OUTPUT_DIMENSIONS = 41

EPSILON_MIN = 0.05
EPSILON_REDUCTION = 0.00005

BUFFER_SAMPLE_SIZE = 128

games = Tetris_AGENT.MultiGame(NUM_GAMES)

model        = DQN(input_dim=INPUT_DIMENSIONS, output_dim=OUTPUT_DIMENSIONS, target_max=SET_TARGET_IN_ACTIONS, number_of_neurons=NUM_NEURONS)
target_model = DQN(input_dim=INPUT_DIMENSIONS, output_dim=OUTPUT_DIMENSIONS, target_max=SET_TARGET_IN_ACTIONS, number_of_neurons=NUM_NEURONS)

logger  = EpisodeLogger(LOG_TRAINING_IN_EPISODES)
watcher = EpisodeWatcher(games=games, watch=WATCH_TRANING)

optimizer = optim.Adam(model.parameters(), lr=0.001)
criterion = nn.MSELoss()

buffer = ReplayBuffer()

games.resetAll()
state = np.array(get_all_current_state(games), dtype=np.float32)

epsilon = 1

action_count = 0
sum_action_count = 0

pieces_placed = 0
pieces_placed_counter = 0

while True:

    action_count = 0

    model.check_if_should_set_new_target(policy_net=model, target_net=target_model)
    
    if (logger.check_should_console_log_episodes(pieces_placed=pieces_placed, lines_cleared=games.getLinesCleared())) :
        sum_action_count = 0

    watcher.check_should_watch_episodes()


    state_t = torch.FloatTensor(state)
    q_values = model(state_t)


    action_count = len(q_values)

    action_indexes = []

    for i in range(action_count) :

        if random.random() > epsilon :
            action_indexes.append(torch.argmax(q_values[i]).item())
        else :
            action_indexes.append(random.randint(0,40))

    action_enums = []

    for i in range(len(action_indexes)) :

        action_enums.append(Tetris_AGENT.PreciseActions(action_indexes[i]))

    step_data = games.stepAll(action_enums)

    next_state = np.array(get_all_current_state(games), dtype=np.float32)

    for i in range(len(step_data)) :

        step_i = step_data[i]

        if step_i.piece_placed :

            model.increase_target_counter()
            action_count += 1

            pieces_placed += 1
            pieces_placed_counter += 1
            if (buffer.size() > 1000) :
                experience_samples = buffer.sample(BUFFER_SAMPLE_SIZE)
                back_propagation(experience_samples)

        if step_i.lost :
            games.resetThis(i)
            logger.increase_log_counter()
            
    for i in range(len(state)) :
        
        buffer.push(state[i], action_enums[i].value, step_data[i].reward, next_state[i], step_data[i].lost)
    
    state = next_state

    if epsilon > EPSILON_MIN :
        epsilon -= EPSILON_REDUCTION
