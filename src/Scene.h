#ifndef SCENE_H
#define SCENE_H

#include <SDL.h>
class Scene {
public:
  Scene() = default;
  virtual ~Scene() = default;

  // 为0时，代表必须去子类实现
  virtual void init() = 0;
  virtual void update(float deltaTime) = 0;
  virtual void render() = 0;
  virtual void clean() = 0;
  virtual void handleEvent(SDL_Event *event) = 0;
};

#endif