
import os
import sys

import random
import numpy as np
from collections import deque
import torch
import torch.nn as nn


agent_path = os.path.abspath("./Release")
sys.path.append(agent_path)

import Tetris_AGENT


class DQN(nn.Module):
    def __init__(self, input_dim, output_dim):
        super(DQN, self).__init__()
        self.net = nn.Sequential(
            nn.Linear(input_dim, 64),
            nn.ReLU(),
            nn.Linear(64, 64),
            nn.ReLU(),
            nn.Linear(64, output_dim)
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


def sigmoid_scale(x, k=5.0):
    return (2.0 / (1.0 + np.exp(-x / k))) - 1.0

def get_current_state(game: Tetris_AGENT.Game) :
    
    agg_height = game.get_aggregate_height()
    holes = game.get_amount_of_holes()
    rugosity = game.get_rugosity()
    current_piece_type = game.get_current_peice_type()
    piece_queue = game.get_piece_queue()

    scaled_height = sigmoid_scale(agg_height, 30)
    scaled_holes = sigmoid_scale(holes, 5)
    scaled_rugosity = sigmoid_scale(rugosity, 10)
    scaled_current_piece = current_piece_type / 6
    
    scaled_piece_queue = []

    for piece in piece_queue :
        scaled_piece_queue.append( piece / 6 )

    concat = np.concatenate([[scaled_height], [scaled_holes], [scaled_rugosity], [scaled_current_piece], scaled_piece_queue])

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


jogo = Tetris_AGENT.Game()

model = DQN(input_dim=9, output_dim=7)

buffer = ReplayBuffer()

for episode in range(1000):
    jogo.reset()
    done = False
    
    state = np.array(get_current_state(jogo), dtype=np.float32)
    
    action_count = 0

    while not done:

        state_t = torch.FloatTensor(state)
        q_values = model(state_t)
        # print("Q value = " + str(q_values))
        action_idx = torch.argmax(q_values).item()
        
        action_enum = Tetris_AGENT.Actions(action_idx)
        
        reward = jogo.step(action_enum)
        
        next_state = np.array(get_current_state(jogo), dtype=np.float32)
        
        if reward <= -100.0:
            done = True
            
        buffer.push(state, action_idx, reward, next_state, done)
        state = next_state

        action_count += 1
        
        if action_count == 100:
            print_current_state(state)
            print("Actions index:      " + str(action_idx))
            print("Reward :           " + str(reward))
            print("Score :             " + str(jogo.get_score()))
            print("-" * 22)
            action_count = 0

        # TODO: Get sample data from experience buffer
        # TODO: Bellman equation optimization

print(buffer)