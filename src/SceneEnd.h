#ifndef SCENE_END_H
#define SCENE_END_H

#include "Scene.h"

class SceneEnd : public Scene {
  // 为0时，代表必须去子类实现
  virtual void init();
  virtual void update(float deltaTime);
  virtual void render();
  virtual void clean();
  virtual void handleEvent(SDL_Event *event);
};

#endif