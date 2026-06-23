import sys
import os

sys.path.append(os.path.abspath("./build"))

import agent_data


os.system('cls')

dc = agent_data.DataContainer()
print(dc.return_data())