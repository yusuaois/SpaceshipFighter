#include "Game.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_mutex.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#include <SDL_ttf.h>
#include <iostream>

int main(int, char **) {
  Game &game = Game::GetInstance(); // Create an instance of the Game class
  game.init();                      // Initialize the game
  game.run();                       // Run the game
  return 0;
}