#include "Game.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>

int main(int, char **) {
  Game &game = Game::GetInstance(); // Create an instance of the Game class
  game.init();                      // Initialize the game
  game.run();                       // Run the game
  return 0;
}