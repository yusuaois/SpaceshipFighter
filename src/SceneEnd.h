#ifndef SCENE_END_H
#define SCENE_END_H

#include "Scene.h"
#include <string>

class SceneEnd : public Scene {
public:
  // 为0时，代表必须去子类实现
  virtual void init();
  virtual void update(float deltaTime);
  virtual void render();
  virtual void clean();
  virtual void handleEvent(SDL_Event *event);

private:
  bool isTyping = true;
  std::string name = "";
  void renderPhase1();
  void renderPhase2();

  void removeLastUtf8Char(std::string &str);
};

#endif