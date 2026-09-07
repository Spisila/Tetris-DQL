
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