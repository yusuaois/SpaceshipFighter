#ifndef OBJECT_H
#define OBJECT_H

#include <SDL_rect.h>
#include <SDL_render.h>

enum class ItemType {
  Life,
  Shield,
  Time
};

// 玩家
struct Player {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  int width = 0;
  int height = 0;
  int speed = 300;
  int curHealth = 3;
  int maxHealth = 3;
  Uint32 coolDown = 300;
  Uint32 lastShotTime = 0;
};
// 玩家子弹
struct ProjectilePlayer {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  int width = 0;
  int height = 0;
  int speed = 600;
  int damage = 1;
};
// 敌人
struct Enemy {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  int width = 0;
  int height = 0;
  int speed = 150;
  int curHealth = 2;
  Uint32 coolDown = 2000;
  Uint32 lastShotTime = 0;
};
// 敌人子弹
struct ProjectileEnemy {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  SDL_FPoint direction = {0, 0};
  int width = 0;
  int height = 0;
  int speed = 400;
  int damage = 1;
};

struct Explosion {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  int width = 0;
  int height = 0;
  int curFrame = 0;
  int totalFrame = 0;
  Uint32 startTime = 0;
  Uint32 FPS = 10;
};

struct Item{
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  SDL_FPoint direction = {0, 0};
  int width = 0;
  int height = 0;
  int speed = 150;
  int bounceCnt = 3;
  ItemType type = ItemType::Life;
};

#endif