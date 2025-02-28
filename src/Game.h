#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "Scene.h"
#include <SDL_events.h>


class Game {
public:
  Game();
  ~Game();
  void run();
  void init();
  void clean();
  void changeScene(Scene *scene);

  void handleEvent(SDL_Event *event);
  void update();
  void render();

private:
  bool isRunning = true;
  Scene *curScene = nullptr;
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  int windowWidth = 600;
  int windowHeight = 800;
};

#endif