#include "SceneMain.h"
#include "Game.h"
#include <SDL.h>
#include <SDL_image.h>
#include <random>

SceneMain::SceneMain() : game(Game::GetInstance()) {}

SceneMain::~SceneMain() {}

void SceneMain::handleEvent(SDL_Event *event) {}

void SceneMain::init() {
  // 随机数
  std::random_device rd;
  gen = std::mt19937(rd());
  dis = std::uniform_real_distribution<float>(0.0f, 1.0f);

  player.texture =
      IMG_LoadTexture(game.getRenderer(), "assets/image/SpaceShip.png");
  SDL_QueryTexture(player.texture, NULL, NULL, &player.width, &player.height);
  player.width /= 4;
  player.height /= 4;
  player.position.x =
      (game.getWindowWidth() - player.width) / 2; // center the player
  player.position.y = game.getWindowHeight() - player.height;

  // 初始化模板
  // 玩家子弹
  projectilePlayerTemplate.texture =
      IMG_LoadTexture(game.getRenderer(), "assets/image/laser-1.png");
  SDL_QueryTexture(projectilePlayerTemplate.texture, NULL, NULL,
                   &projectilePlayerTemplate.width,
                   &projectilePlayerTemplate.height);
  projectilePlayerTemplate.width /= 4;
  projectilePlayerTemplate.height /= 4;
  // 敌机
  enemyTemplate.texture =
      IMG_LoadTexture(game.getRenderer(), "assets/image/insect-2.png");
  SDL_QueryTexture(enemyTemplate.texture, NULL, NULL, &enemyTemplate.width,
                   &enemyTemplate.height);
  enemyTemplate.width /= 4;
  enemyTemplate.height /= 4;
}

void SceneMain::update(float deltaTime) {
  keyBoardControl(deltaTime);
  updateProjectiles(deltaTime);

  spawnEnemies();
  updateEnemies(deltaTime);
}

void SceneMain::render() {
  // 渲染玩家子弹
  renderProjectiles();
  // 渲染玩家
  SDL_Rect playerRect = {static_cast<int>(player.position.x),
                         static_cast<int>(player.position.y), player.width,
                         player.height};
  SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &playerRect);

  // 渲染敌机
  renderEnemies();
}
void SceneMain::clean() {
  // 清理容器
  // 清理玩家子弹
  for (auto &projectile : ProjectilesPlayer) {
    if (projectile) {
      delete (projectile);
    }
  }
  ProjectilesPlayer.clear();
  // 清理敌机
  for (auto &enemy : Enemies) {
    if (enemy) {
      delete (enemy);
    }
  }
  Enemies.clear();

  // 清理模板
  if (player.texture)
    SDL_DestroyTexture(player.texture);
  if (projectilePlayerTemplate.texture)
    SDL_DestroyTexture(projectilePlayerTemplate.texture);
  if (enemyTemplate.texture)
    SDL_DestroyTexture(enemyTemplate.texture);
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

void SceneMain::spawnEnemies() {
  if (dis(gen) > 1.0f / 60.0f)
    return;

  Enemy *enemy = new Enemy(enemyTemplate);
  enemy->position.x = dis(gen) * (game.getWindowWidth() - enemy->width);
  enemy->position.y = -enemy->height;
  Enemies.push_back(enemy);
}

void SceneMain::updateEnemies(float deltaTime) {
  for (auto it = Enemies.begin(); it != Enemies.end();) {
    auto Enemy = *it;
    Enemy->position.y += deltaTime * Enemy->speed;
    if (Enemy->position.y > game.getWindowHeight()) {
      delete Enemy;
      it = Enemies.erase(it);
    } else {
      ++it;
    }
  }
}

void SceneMain::renderEnemies() {
  for (auto Enemy : Enemies) {
    SDL_Rect enemyRect = {
        static_cast<int>(Enemy->position.x),
        static_cast<int>(Enemy->position.y),
        static_cast<int>(Enemy->width),
        static_cast<int>(Enemy->height),
    };
    SDL_RenderCopy(game.getRenderer(), Enemy->texture, nullptr, &enemyRect);
  }
}