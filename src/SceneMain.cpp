#include "SceneMain.h"
#include "Game.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_keyboard.h>
#include <SDL_render.h>
#include <cstddef>

SceneMain::SceneMain() : game(Game::GetInstance()) {}

SceneMain::~SceneMain() {}
void SceneMain::handleEvent(SDL_Event *event) {}
void SceneMain::init() {
  player.texture =
      IMG_LoadTexture(game.getRenderer(), "assets/image/SpaceShip.png");
  SDL_QueryTexture(player.texture, NULL, NULL, &player.width, &player.height);
  player.width /= 4;
  player.height /= 4;
  player.position.x =
      (game.getWindowWidth() - player.width) / 2; // center the player
  player.position.y = game.getWindowHeight() - player.height;
}
void SceneMain::update() {
  keyBoardControl();
}

void SceneMain::render() {
  SDL_Rect playerRect = {static_cast<int>(player.position.x),
                         static_cast<int>(player.position.y), player.width,
                         player.height};
  SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &playerRect);
}
void SceneMain::clean() { SDL_DestroyTexture(player.texture); }

void SceneMain::keyBoardControl(){
  auto keyBoardState = SDL_GetKeyboardState(NULL);
  if (keyBoardState[SDL_SCANCODE_W]&& player.position.y > 0) {
    player.position.y -= 1;
  }
  if (keyBoardState[SDL_SCANCODE_S]&& player.position.y < game.getWindowHeight() - player.height) {
    player.position.y += 1;
  }
  if (keyBoardState[SDL_SCANCODE_A]&& player.position.x > 0) {
    player.position.x -= 1;
  }
  if (keyBoardState[SDL_SCANCODE_D]&& player.position.x < game.getWindowWidth() - player.width) {
    player.position.x += 1;
  }
}
