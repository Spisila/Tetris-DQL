
import os
import sys


agent_path = os.path.abspath("./Release")
sys.path.append(agent_path)

import Tetris_AGENT

test = Tetris_AGENT.Game()

for i in range(10) :

  print(test.step(Tetris_AGENT.Actions.HARD_DROP))

  print(test.get_rugosity())