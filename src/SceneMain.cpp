#include "SceneMain.h"
#include "Game.h"
#include "Object.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_log.h>
#include <SDL_mixer.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#include <cstddef>
#include <random>

SceneMain::SceneMain() : game(Game::GetInstance()) {}

SceneMain::~SceneMain() {}

void SceneMain::handleEvent(SDL_Event *event) {}

void SceneMain::init() {
  // 音乐
  bgm = Mix_LoadMUS("assets/music/03_Racing_Through_Asteroids_Loop.ogg");
  if (bgm == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music: %s",
                 Mix_GetError);
  }
  Mix_PlayMusic(bgm, -1);

  // 读取音效资源
  sounds["player_shoot"] = Mix_LoadWAV("assets/sound/laser_shoot4.wav");
  sounds["enemy_shoot"] = Mix_LoadWAV("assets/sound/xs_laser.wav");
  sounds["player_explosion"] = Mix_LoadWAV("assets/sound/explosion1.wav");
  sounds["enemy_explosion"] = Mix_LoadWAV("assets/sound/explosion2.wav");
  sounds["hit"] = Mix_LoadWAV("assets/sound/eff11.wav");
  sounds["getItem"] = Mix_LoadWAV("assets/sound/eff5.wav");

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
  // 爆炸
  explosionTemplate.texture =
      IMG_LoadTexture(game.getRenderer(), "assets/effect/explosion.png");
  SDL_QueryTexture(explosionTemplate.texture, NULL, NULL,
                   &explosionTemplate.width, &explosionTemplate.height);
  explosionTemplate.totalFrame =
      explosionTemplate.width / explosionTemplate.height;
  explosionTemplate.width = explosionTemplate.height;
  // 物品掉落
  itemLifeTemplate.texture =
      IMG_LoadTexture(game.getRenderer(), "assets/image/bonus_life.png");
  SDL_QueryTexture(itemLifeTemplate.texture, NULL, NULL,
                   &itemLifeTemplate.width, &itemLifeTemplate.height);
  itemLifeTemplate.width /= 4;
  itemLifeTemplate.height /= 4;
}

void SceneMain::update(float deltaTime) {

  keyBoardControl(deltaTime);
  updateProjectiles(deltaTime);

  spawnEnemies();
  updateEnemies(deltaTime);

  updateEnemyProjectiles(deltaTime);
  updatePlayer(deltaTime);

  updateExplosions(deltaTime);

  updateItems(deltaTime);
}

void SceneMain::render() {
  // 渲染玩家子弹
  renderProjectiles();
  // 渲染敌机子弹
  renderEnemyProjectiles();
  // 渲染玩家
  if (!isDead) {
    SDL_Rect playerRect = {static_cast<int>(player.position.x),
                           static_cast<int>(player.position.y), player.width,
                           player.height};
    SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &playerRect);
  }
  // 渲染敌机
  renderEnemies();
  // 渲染物品
  renderItems();
  // 渲染爆炸效果
  renderExplosions();
}
void SceneMain::clean() {
  // 清理容器
  // 清理声音
  for (auto sound : sounds) {
    if (sound.second != nullptr) {
      Mix_FreeChunk(sound.second);
    }
  }
  sounds.clear();
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
  // 清理爆炸效果
  for (auto &explosion : Explosions) {
    if (explosion) {
      delete (explosion);
    }
  }
  Explosions.clear();
  // 清理物品
  for (auto &item : Items) {
    if (item) {
      delete (item);
    }
  }

  // 清理模板
  if (player.texture)
    SDL_DestroyTexture(player.texture);
  if (projectilePlayerTemplate.texture)
    SDL_DestroyTexture(projectilePlayerTemplate.texture);
  if (enemyTemplate.texture)
    SDL_DestroyTexture(enemyTemplate.texture);
  if (projectileEnemyTemplate.texture)
    SDL_DestroyTexture(projectileEnemyTemplate.texture);
  if (explosionTemplate.texture)
    SDL_DestroyTexture(explosionTemplate.texture);
  if (itemLifeTemplate.texture)
    SDL_DestroyTexture(itemLifeTemplate.texture);

  // 清理音乐
  if (bgm != nullptr) {
    Mix_HaltMusic();
    Mix_FreeMusic(bgm);
  }
}

void SceneMain::keyBoardControl(float deltaTime) {
  if (isDead)
    return;
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
  Mix_PlayChannel(0, sounds["player_shoot"], 0);
}

void SceneMain::updatePlayer(float) {
  if (isDead)
    return;
  if (player.curHealth <= 0) {
    // TODO gameover
    int curTime = SDL_GetTicks();
    isDead = true;
    auto explode = new Explosion(explosionTemplate);
    explode->position.x =
        player.position.x + player.width / 2 - explode->width / 2;
    explode->position.y =
        player.position.y + player.height / 2 - explode->height / 2;
    explode->startTime = curTime;
    Explosions.push_back(explode);
    Mix_PlayChannel(-1, sounds["player_explosion"], 0);
    return;
  }
  for (auto enemy : Enemies) {
    SDL_Rect enemyRect = {static_cast<int>(enemy->position.x),
                          static_cast<int>(enemy->position.y), enemy->width,
                          enemy->height};

    SDL_Rect playerRect = {static_cast<int>(player.position.x),
                           static_cast<int>(player.position.y), player.width,
                           player.height};

    if (SDL_HasIntersection(&enemyRect, &playerRect)) {
      player.curHealth -= 1;
      enemy->curHealth = 0;
    }
  }
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
          Mix_PlayChannel(-1, sounds["hit"], 0);
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
      if (curTime - Enemy->lastShotTime >= Enemy->coolDown && !isDead) {
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
  Mix_PlayChannel(-1, sounds["enemy_shoot"], 0);
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
      SDL_Rect playerRect = {static_cast<int>(player.position.x),
                             static_cast<int>(player.position.y), player.width,
                             player.height};
      SDL_Rect projectileRect = {static_cast<int>(projectile->position.x),
                                 static_cast<int>(projectile->position.y),
                                 projectile->width, projectile->height};
      if (SDL_HasIntersection(&playerRect, &projectileRect) && !isDead) {
        player.curHealth -= projectile->damage;
        delete projectile;
        it = ProjectilesEnemies.erase(it);
        Mix_PlayChannel(-1, sounds["hit"], 0);
      } else {
        ++it;
      }
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

void SceneMain::enemyExplode(Enemy *enemy) {
  auto curTime = SDL_GetTicks();
  auto explosion = new Explosion(explosionTemplate);
  explosion->position.x =
      enemy->position.x + enemy->width / 2 - explosion->width / 2;
  explosion->position.y =
      enemy->position.y + enemy->height / 2 - explosion->height / 2;
  explosion->startTime = curTime;
  Explosions.push_back(explosion);
  Mix_PlayChannel(-1, sounds["enemy_explode"], 0);
  if (dis(gen) < 0.5f)
    dropItem(enemy);
  delete enemy;
}

void SceneMain::dropItem(Enemy *enemy) {
  auto item = new Item(itemLifeTemplate);
  item->position.x = enemy->position.x + enemy->width / 2 - item->width / 2;
  item->position.y = enemy->position.y + enemy->height / 2 - item->height / 2;
  float angle = dis(gen) * 2 * M_PI;
  item->direction.x = cos(angle);
  item->direction.y = sin(angle);
  Items.push_back(item);
}

void SceneMain::updateExplosions(float) {
  auto curTime = SDL_GetTicks();
  for (auto it = Explosions.begin(); it != Explosions.end();) {
    auto explosion = *it;
    explosion->curFrame =
        (curTime - explosion->startTime) * explosion->FPS / 1000;
    if (explosion->curFrame >= explosion->totalFrame) {
      delete explosion;
      it = Explosions.erase(it);
    } else {
      ++it;
    }
  }
}

void SceneMain::renderExplosions() {
  for (auto explosion : Explosions) {
    SDL_Rect src = {explosion->curFrame * explosion->width, 0, explosion->width,
                    explosion->height};
    SDL_Rect dst = {static_cast<int>(explosion->position.x),
                    static_cast<int>(explosion->position.y), explosion->width,
                    explosion->height};
    SDL_RenderCopy(game.getRenderer(), explosion->texture, &src, &dst);
  }
}

void SceneMain::updateItems(float deltaTime) {
  for (auto it = Items.begin(); it != Items.end();) {
    auto item = *it;
    // 更新位置
    item->position.x += item->direction.x * item->speed * deltaTime;
    item->position.y += item->direction.y * item->speed * deltaTime;
    if (item->bounceCnt > 0) {
      // 处理屏幕边缘反弹
      if (item->position.x < 0) {
        item->direction.x = -item->direction.x;
        item->bounceCnt--;
      }
      if (item->position.x + item->width > game.getWindowWidth()) {
        item->direction.x = -item->direction.x;
        item->bounceCnt--;
      }
      if (item->position.y < 0) {
        item->direction.y = -item->direction.y;
        item->bounceCnt--;
      }
      if (item->position.y + item->height > game.getWindowHeight()) {
        item->direction.y = -item->direction.y;
        item->bounceCnt--;
      }
    }
    // 超出屏幕外删除
    if (item->position.y + item->height < 0 ||
        item->position.y > game.getWindowHeight() ||
        item->position.x + item->width < 0 ||
        item->position.x > game.getWindowWidth()) {
      delete item;
      it = Items.erase(it);
    } else {
      SDL_Rect itemRect = {static_cast<int>(item->position.x),
                           static_cast<int>(item->position.y), item->width,
                           item->height};
      SDL_Rect playerRect = {static_cast<int>(player.position.x),
                             static_cast<int>(player.position.y), player.width,
                             player.height};
      if (SDL_HasIntersection(&itemRect, &playerRect) && isDead == false) {
        playerGetItem(item);
        delete item;
        it = Items.erase(it);
      } else {
        ++it;
      }
    }
  }
}

void SceneMain::playerGetItem(Item *item) {
  Mix_PlayChannel(-1, sounds["getItem"], 0);
  if (item->type == ItemType::Life && player.curHealth < player.maxHealth) {
    player.curHealth++;
  }
}

void SceneMain::renderItems() {
  for (auto &item : Items) {
    SDL_Rect itemRect = {static_cast<int>(item->position.x),
                         static_cast<int>(item->position.y), item->width,
                         item->height};
    SDL_RenderCopy(game.getRenderer(), item->texture, nullptr, &itemRect);
  }
}