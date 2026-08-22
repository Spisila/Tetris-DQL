
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

from Debug import Tetris_AGENT

class DQN(nn.Module):

    set_target_counter = 0

    def __init__(self, input_dim, output_dim, target_max):

        self.set_target_max = target_max

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

    @classmethod
    def increase_target_counter(cls) :
        cls.set_target_counter += 1

    @classmethod
    def reset_target_counter(cls) :
        cls.set_target_counter = 0


    def check_if_should_set_new_target(self, policy_net, target_net) :

        if DQN.set_target_counter >= self.set_target_max :
            target_net.load_state_dict(policy_net.state_dict())
            DQN.reset_target_counter()

        

class ReplayBuffer:
    def __init__(self, capacity=50000):
        self.buffer = deque(maxlen=capacity)
        
    def push(self, state, action, reward, next_state, done):
        self.buffer.append((state, action, reward, next_state, done))
        
    def sample(self, batch_size):
        return random.sample(self.buffer, batch_size)
    
    def size(self) :
        return len(self.buffer)

class EpisodeLogger:

    log_counter = 0

    episode_count = 0
    pieces_placed_sum = 0
    lines_cleared_sum = 0

    def __init__(self, log_counter_max):
        self.log_counter_max = log_counter_max

    @classmethod
    def increase_log_counter(cls) :
        cls.log_counter += 1
        cls.episode_count += 1

    @classmethod
    def reset_log_counter(cls) :
        cls.log_counter = 0

    @classmethod
    def console_log_episode(cls, pieces_placed, lines_cleared) :

        cls.pieces_placed_sum += pieces_placed
        cls.lines_cleared_sum += lines_cleared

        print("EPISODE = "                   + str(cls.episode_count)     + 
            " | PIECES PLACED = "            + str(cls.pieces_placed_sum) + 
            " | PIECES PLACED IN EPISODE = " + str(pieces_placed) +
            " | LINES = "                    + str(cls.lines_cleared_sum) +
            " | LINES CLEARED IN EPISODE = " + str(lines_cleared))
        cls.reset_log_counter()

    def check_should_console_log_episodes(self, pieces_placed, lines_cleared) :

        if EpisodeLogger.log_counter >= self.log_counter_max :
            self.console_log_episode(pieces_placed=pieces_placed, lines_cleared=lines_cleared)
            return True
        return False
        

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
set_target_in_actions = 500

games = Tetris_AGENT.MultiGame(32)

model = DQN(input_dim=39, output_dim=41, target_max=set_target_in_actions)
target_model = DQN(input_dim=39, output_dim=41, target_max=set_target_in_actions)

logger = EpisodeLogger(100)

optimizer = optim.Adam(model.parameters(), lr=0.001)
criterion = nn.MSELoss()

buffer = ReplayBuffer()

games.resetAll()
state = np.array(get_all_current_state(games), dtype=np.float32)

done = False
epsilon = 1

action_count = 0
sum_action_count = 0

pieces_placed = 0
pieces_placed_counter = 0

watch_counter = 0
watch_episode_counter = 20

graphics_init = False

finished_watch_counter = 0
finished_watch_max = 20

games.initGraphics()

while True:

    action_count = 0

    games.render(0)

    model.check_if_should_set_new_target(policy_net=model, target_net=target_model)
    
    if (logger.check_should_console_log_episodes(pieces_placed=pieces_placed, lines_cleared=games.getLinesCleared())) :
        sum_action_count = 0
        watch_counter += 1

    # if watch_counter >= 10 :
    #     if graphics_init == False :
    #         graphics_init = True
    #         games.initGraphics()
    #     watch_counter = 0

    if graphics_init == True :
        games.render(0)

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

        if step_i.piece_placed == True :

            model.increase_target_counter()
            action_count += 1

            pieces_placed += 1
            pieces_placed_counter += 1
            if (buffer.size() > 1000) :
                experience_samples = buffer.sample(128)
                back_propagation(experience_samples)

        if step_i.lost == True :
            games.resetThis(i)
            logger.increase_log_counter()
            
    for i in range(len(state)) :
        
        buffer.push(state[i], action_enums[i].value, step_data[i].reward, next_state[i], step_data[i].lost)
    
    state = next_state

    if epsilon > 0.05 :
        epsilon -= 0.00005
