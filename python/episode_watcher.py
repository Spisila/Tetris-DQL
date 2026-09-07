

class EpisodeWatcher() :

  def __init__(self, games, watch=True):
    self.games = games

    self.window_initialized = False
    self.should_watch = watch

  def init_window(self) :
    self.games.initGraphics()
    self.window_initialized = True

  def render_game(self) :
    self.games.render("0")

  def close_graphics(self) :
    self.games.closeGraphics()
    self.window_initialized = False

  def check_should_watch_episodes(self) :

    if self.should_watch :

      if not self.window_initialized :
        self.init_window()

      if self.window_initialized :
        self.render_game()  
