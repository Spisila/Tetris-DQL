
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

def get_current_state(game: Tetris_AGENT.Game) :
    
    agg_height = game.get_aggregate_height()
    holes = game.get_amount_of_holes()
    rugosity = game.get_rugosity()
    current_piece_type = game.get_current_peice_type()
    piece_queue = game.get_piece_queue()

    formated_queue = []
    for piece in piece_queue :
        match piece :
            case 0 : 
                formated_queue.append(0)
            case 4 :
                formated_queue.append(1)
            case 8 :
                formated_queue.append(2)
            case 12 :
                formated_queue.append(3)
            case 16 :
                formated_queue.append(4)
            case 20 :
                formated_queue.append(5)
            case 24 :
                formated_queue.append(6)


    scaled_height = sigmoid_scale(agg_height, 30)
    scaled_holes = sigmoid_scale(holes, 5)
    scaled_rugosity = sigmoid_scale(rugosity, 10)
    scaled_current_piece = current_piece_type / 6
    
    queue_tensor = torch.tensor(formated_queue)

    hot_one_piece_queue = F.one_hot(queue_tensor, num_classes=7)

    flat_hot_one = hot_one_piece_queue.flatten()

    # TODO: Pass piece queue as hotone encoding

    concat = np.concatenate([[scaled_height], [scaled_holes], [scaled_rugosity], [scaled_current_piece], flat_hot_one])

    return concat

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

game = Tetris_AGENT.Game()

model = DQN(input_dim=39, output_dim=7)

target_model = DQN(input_dim=39, output_dim=7)

optimizer = optim.Adam(model.parameters(), lr=0.001)
criterion = nn.MSELoss()

buffer = ReplayBuffer()

# game.init_graphics()

graphics_init = False

epsilon = 1

sum_action_count = 0

watch_generation_counter = 20

pieces_placed = 0

for generation in range(10_000_000):
    game.reset()
    done = False
    
    state = np.array(get_current_state(game), dtype=np.float32)

    action_count = 0


    if generation % 10000 == 0 :
        if graphics_init == False :
            graphics_init = True
            game.init_graphics()

    if graphics_init == True :
        watch_generation_counter -= 1

    if watch_generation_counter <= 0:
        watch_generation_counter = 20
        graphics_init = False
        game.close_graphics()

    if pieces_placed % 1000 == 0:
        target_model.load_state_dict(model.state_dict())

    if generation % 500 == 0 and generation > 0:
        print("GENERATION = " + str(generation) + " | SUM ACTIONS = " + str(sum_action_count) + " | SCORE = " + str(game.get_score()))
        sum_action_count = 0


    if epsilon > 0.05 :
        epsilon -= 0.0001


    action_count = 0

    while not done:

        state_t = torch.FloatTensor(state)
        q_values = model(state_t)

        if random.random() > epsilon :
            action_idx = torch.argmax(q_values).item()
        else :
            action_idx = random.randint(0,6)


        action_enum = Tetris_AGENT.Actions(action_idx)
        
        reward = game.step(action_enum)
        
        next_state = np.array(get_current_state(game), dtype=np.float32)
        
        if game.lost == True:
            done = True
            
        buffer.push(state, action_idx, reward, next_state, done)
        state = next_state

        
        action_count += 1
        sum_action_count += 1


        if game.piece_set == True :
            pieces_placed += 1
            if (buffer.size() > 100) :
                experience_samples = buffer.sample(64)
                back_propagation(experience_samples)

        if graphics_init == True :
            game.render(str(generation))


game.close_graphics()