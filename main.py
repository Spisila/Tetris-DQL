
import os
import sys
import time

import random
import numpy as np
from collections import deque
import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F


agent_path = os.path.abspath("./Release")
sys.path.append(agent_path)

import Tetris_AGENT

NUM_NEURONS = 128

class DQN(nn.Module):
    def __init__(self, input_dim, output_dim):
        super(DQN, self).__init__()
        self.net = nn.Sequential(
            nn.Linear(input_dim, NUM_NEURONS),
            nn.ReLU(),
            nn.Linear(NUM_NEURONS, NUM_NEURONS),
            nn.ReLU(),
            nn.Linear(NUM_NEURONS, output_dim)
        )
    def forward(self, x):
        return self.net(x)

class ReplayBuffer:
    def __init__(self, capacity=50000):
        self.buffer = deque(maxlen=capacity)
        
    def push(self, state, action, reward, next_state, done):
        self.buffer.append((state, action, reward, next_state, done))
        
    def sample(self, batch_size):
        return random.sample(self.buffer, batch_size)
    
    def size(self) :
        return len(self.buffer)


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

games = Tetris_AGENT.MultiGame(128)

model = DQN(input_dim=39, output_dim=7)
target_model = DQN(input_dim=39, output_dim=7)

optimizer = optim.Adam(model.parameters(), lr=0.001)
criterion = nn.MSELoss()

buffer = ReplayBuffer()

epsilon = 1

sum_action_count = 0

watch_generation_counter = 20

pieces_placed = 0
pieces_placed_counter = 0

games.resetAll()
done = False

state = np.array(get_all_current_state(games), dtype=np.float32)

action_count = 0

generation = 0

generation_log_counter = 0

while True:

    action_count = 0

    if pieces_placed_counter >= 500 :
        target_model.load_state_dict(model.state_dict())
        pieces_placed_counter = 0

    if generation_log_counter >= 100 and generation > 0:
        print("GENERATION = " + str(generation) + " | PIECES PLACED = " + str(pieces_placed) + " | SUM SCORE = " + str(games.getSumScore()))
        generation_log_counter = 0
        sum_action_count = 0

    state_t = torch.FloatTensor(state)
    q_values = model(state_t)


    action_count = len(q_values)

    action_indexes = []

    for i in range(action_count) :

        if random.random() > epsilon :
            action_indexes.append(torch.argmax(q_values[i]).item())
        else :
            action_indexes.append(random.randint(0,6))

    action_enums = []

    for i in range(len(action_indexes)) :

        action_enums.append(Tetris_AGENT.Actions(action_indexes[i]))
    
    step_data = games.stepAll(action_enums)

    next_state = np.array(get_all_current_state(games), dtype=np.float32)


    for i in range(len(step_data)) :

        step_i = step_data[i]

        if step_i.piece_placed == True :
            action_count += 1


            pieces_placed += 1
            pieces_placed_counter += 1
            if (buffer.size() > 1000) :
                experience_samples = buffer.sample(128)
                back_propagation(experience_samples)

        if step_i.lost == True :
            games.resetThis(i)
            generation += 1
            generation_log_counter += 1
            


    for i in range(len(state)) :
        
        buffer.push(state[i], action_enums[i].value, step_data[i].reward, next_state[i], step_data[i].lost)
    
    state = next_state

    if epsilon > 0.05 :
        epsilon -= 0.00005

