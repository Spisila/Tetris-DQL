
import os
import csv

class EpisodeLogger:

    log_counter = 0

    episode_count = 0
    pieces_placed_sum = 0
    lines_cleared_sum = 0

    def __init__(self, log_counter_max, log_path):
        self.log_counter_max = log_counter_max
        self.log_path = log_path

        
        os.makedirs(os.path.dirname(log_path), exist_ok=True)
        
        with open(self.log_path, mode='w', newline='') as f:
            writer = csv.writer(f)
            action_headers = [f"a_{i}_count" for i in range(41)]
            writer.writerow(["episode_interval", "placed_total", "pieces_placed_interval", "lines_total", "lines_interval"] + action_headers)

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

        print("EPISODE = "                            + str(cls.episode_count)     + 
            " | TOTAL PIECES PLACED = "               + str(cls.pieces_placed_sum) + 
            " | PIECES PLACED IN EPISODE INTERVAL = " + str(pieces_placed) +
            " | TOTAL LINES = "                       + str(cls.lines_cleared_sum) +
            " | LINES CLEARED IN EPISODE INTERVAL = " + str(lines_cleared))
        cls.reset_log_counter()


    def csv_log_episode(cls, pieces_placed, lines_cleared) :
        row = [cls.episode_count, cls.pieces_placed_sum, pieces_placed, cls.lines_cleared_sum, lines_cleared]
        
        with open(cls.log_path, mode='a', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(row)

    def check_should_console_log_episodes(self, pieces_placed, lines_cleared) :

        if EpisodeLogger.log_counter >= self.log_counter_max :
            self.console_log_episode(pieces_placed=pieces_placed, lines_cleared=lines_cleared)
            self.csv_log_episode(pieces_placed=pieces_placed, lines_cleared=lines_cleared)
            return True
        return False