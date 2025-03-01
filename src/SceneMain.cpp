#include "SceneMain.h"
#include "Game.h"
#include "Object.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_keyboard.h>
#include <SDL_rect.h>
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

  // 初始化模板
  projectilePlayerTemplate.texture =
      IMG_LoadTexture(game.getRenderer(), "assets/image/laser-1.png");
  SDL_QueryTexture(projectilePlayerTemplate.texture, NULL, NULL,
                   &projectilePlayerTemplate.width,
                   &projectilePlayerTemplate.height);
  projectilePlayerTemplate.width /= 4;
  projectilePlayerTemplate.height /= 4;
}
void SceneMain::update(float deltaTime) {
  keyBoardControl(deltaTime);
  updateProjectiles(deltaTime);
}

void SceneMain::render() {
  // 渲染玩家子弹
  renderProjectiles();
  // 渲染玩家
  SDL_Rect playerRect = {static_cast<int>(player.position.x),
                         static_cast<int>(player.position.y), player.width,
                         player.height};
  SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &playerRect);
}
void SceneMain::clean() {
  // 清理容器
  for (auto &projectile : ProjectilesPlayer) {
    if (projectile) {
      delete (projectile);
    }
  }
  ProjectilesPlayer.clear();
  // 清理模板
  if (player.texture)
    SDL_DestroyTexture(player.texture);
  if (projectilePlayerTemplate.texture)
    SDL_DestroyTexture(projectilePlayerTemplate.texture);
}

void SceneMain::keyBoardControl(float deltaTime) {
  auto keyBoardState = SDL_GetKeyboardState(NULL);
  if (keyBoardState[SDL_SCANCODE_W] && player.position.y > 0) {
    player.position.y -= deltaTime * player.speed;
  }
  if (keyBoardState[SDL_SCANCODE_S] &&
      player.position.y < game.getWindowHeight() - player.height) {
    player.position.y += deltaTime * player.speed;
  }
  if (keyBoardState[SDL_SCANCODE_A] && player.position.x > 0) {
    player.position.x -= deltaTime * player.speed;
  }
  if (keyBoardState[SDL_SCANCODE_D] &&
      player.position.x < game.getWindowWidth() - player.width) {
    player.position.x += deltaTime * player.speed;
  }
  if (keyBoardState[SDL_SCANCODE_J]) {
    auto currentTime = SDL_GetTicks();
    if (currentTime - player.lastShotTime > player.coolDown) {
      shootPlayer();
      player.lastShotTime = currentTime;
    }
  }
}
void SceneMain::shootPlayer() {
  auto Projectile = new ProjectilePlayer(projectilePlayerTemplate);
  Projectile->position.x =
      player.position.x + player.width / 2 - Projectile->width / 2;
  Projectile->position.y = player.position.y;
  ProjectilesPlayer.push_back(Projectile);
}

void SceneMain::updateProjectiles(float deltaTime) {
  int margin = 32;
  for (auto it = ProjectilesPlayer.begin(); it != ProjectilesPlayer.end();) {
    auto Projectile = *it;
    Projectile->position.y -= deltaTime * Projectile->speed;
    if (Projectile->position.y + margin < 0) {
      delete Projectile;
      it = ProjectilesPlayer.erase(it);
    } else {
      ++it;
    }
  }
}

void SceneMain::renderProjectiles() {
  for (auto Projectile : ProjectilesPlayer) {
    SDL_Rect projectileRect = {
        static_cast<int>(Projectile->position.x),
        static_cast<int>(Projectile->position.y),
        static_cast<int>(Projectile->width),
        static_cast<int>(Projectile->height),
    };
    SDL_RenderCopy(game.getRenderer(), Projectile->texture, nullptr,
                   &projectileRect);
  }
}