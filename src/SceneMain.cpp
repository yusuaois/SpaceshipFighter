#include "SceneMain.h"
#include "Game.h"
#include "Object.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_timer.h>
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
  // 敌机子弹
  projectileEnemyTemplate.texture =
      IMG_LoadTexture(game.getRenderer(), "assets/image/bullet-1.png");
  SDL_QueryTexture(projectileEnemyTemplate.texture, NULL, NULL,
                   &projectileEnemyTemplate.width,
                   &projectileEnemyTemplate.height);
  projectileEnemyTemplate.width /= 4;
  projectileEnemyTemplate.height /= 4;
}

void SceneMain::update(float deltaTime) {
  keyBoardControl(deltaTime);
  updateProjectiles(deltaTime);

  spawnEnemies();
  updateEnemies(deltaTime);

  updateEnemyProjectiles(deltaTime);
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
  // 渲染敌机子弹
  renderEnemyProjectiles();
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
  // 清理敌机子弹
  for (auto &projectile : ProjectilesEnemies) {
    if (projectile) {
      delete (projectile);
    }
  }
  ProjectilesEnemies.clear();

  // 清理模板
  if (player.texture)
    SDL_DestroyTexture(player.texture);
  if (projectilePlayerTemplate.texture)
    SDL_DestroyTexture(projectilePlayerTemplate.texture);
  if (enemyTemplate.texture)
    SDL_DestroyTexture(enemyTemplate.texture);
  if (projectileEnemyTemplate.texture)
    SDL_DestroyTexture(projectileEnemyTemplate.texture);
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
      bool hit = false;
      for (auto Enemy : Enemies) {
        SDL_Rect enemyRect = {static_cast<int>(Enemy->position.x),
                              static_cast<int>(Enemy->position.y),
                              (Enemy->width), (Enemy->height)};
        SDL_Rect projectileRect = {static_cast<int>(Projectile->position.x),
                                   static_cast<int>(Projectile->position.y),
                                   (Projectile->width), (Projectile->height)};
        if (SDL_HasIntersection(&projectileRect, &enemyRect)) {
          Enemy->curHealth -= Projectile->damage;
          delete Projectile;
          it = ProjectilesPlayer.erase(it);
          hit = true;
          break;
        }
      }
      if (!hit) {
        ++it;
      }
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
  auto curTime = SDL_GetTicks();
  for (auto it = Enemies.begin(); it != Enemies.end();) {
    auto Enemy = *it;
    Enemy->position.y += deltaTime * Enemy->speed;
    if (Enemy->position.y > game.getWindowHeight()) {
      delete Enemy;
      it = Enemies.erase(it);
    } else {
      if (curTime - Enemy->lastShotTime >= Enemy->coolDown) {
        shootEnemy(Enemy);
        Enemy->lastShotTime = curTime;
      }
      if (Enemy->curHealth <= 0) {
        enemyExplode(Enemy);
        it = Enemies.erase(it);
      } else {
        ++it;
      }
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

void SceneMain::shootEnemy(Enemy *enemy) {
  auto projectile = new ProjectileEnemy(projectileEnemyTemplate);
  projectile->position.x =
      enemy->position.x + enemy->width / 2 - projectile->width / 2;
  projectile->position.y =
      enemy->position.y + enemy->height / 2 - projectile->height / 2;
  projectile->direction = getDirectionVector(enemy);
  ProjectilesEnemies.push_back(projectile);
}

SDL_FPoint SceneMain::getDirectionVector(Enemy *enemy) {
  auto x = (player.position.x + player.width / 2) -
           (enemy->position.x + enemy->width / 2);
  auto y = (player.position.y + player.height / 2) -
           (enemy->position.y + enemy->height / 2);
  auto length = sqrt(x * x + y * y);
  x /= length;
  y /= length;
  return SDL_FPoint{x, y};
}

void SceneMain::updateEnemyProjectiles(float deltaTime) {
  auto margin = 32;
  for (auto it = ProjectilesEnemies.begin(); it != ProjectilesEnemies.end();) {
    auto projectile = *it;
    projectile->position.x +=
        projectile->direction.x * projectile->speed * deltaTime;
    projectile->position.y +=
        projectile->direction.y * projectile->speed * deltaTime;
    if (projectile->position.x < -margin ||
        projectile->position.x > game.getWindowWidth() + margin ||
        projectile->position.y < -margin ||
        projectile->position.y > game.getWindowHeight() + margin) {
      delete projectile;
      it = ProjectilesEnemies.erase(it);
    } else {
      ++it;
    }
  }
}

void SceneMain::renderEnemyProjectiles() {
  for (auto projectile : ProjectilesEnemies) {
    SDL_Rect rect = {static_cast<int>(projectile->position.x),
                     static_cast<int>(projectile->position.y),
                     projectile->width, projectile->height};
    float angle =
        atan2(projectile->direction.y, projectile->direction.x) * 180 / M_PI -
        90;
    SDL_RenderCopyEx(game.getRenderer(), projectile->texture, nullptr, &rect,
                     angle, nullptr, SDL_FLIP_NONE);
  }
}

void SceneMain::enemyExplode(Enemy *enemy) { delete enemy; }