#ifndef OBJECT_H
#define OBJECT_H

#include <SDL_rect.h>
#include <SDL_render.h>
// 玩家
struct Player {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  int width = 0;
  int height = 0;
  int speed = 300;
  Uint32 coolDown = 500;
  Uint32 lastShotTime = 0;
};
// 玩家子弹
struct ProjectilePlayer {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  int width = 0;
  int height = 0;
  int speed = 400;
};
// 敌人
struct Enemy {
  SDL_Texture *texture = nullptr;
  SDL_FPoint position = {0, 0};
  int width = 0;
  int height = 0;
  int speed = 200;
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
};

#endif