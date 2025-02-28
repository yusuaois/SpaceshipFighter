#ifndef SCENE_MAIN_H
#define SCENE_MAIN_H

#include "Scene.h"
#include <SDL.h>

class SceneMain : public Scene {
public:
  SceneMain();
  ~SceneMain();

  void update() override;                       // Update the scene
  void render() override;                       // Render the scene
  void handleEvent(SDL_Event *event) override; // Handle events
  void init() override;  // Called when the scene is entered
  void clean() override; // Called when the scene is exited
};

#endif // SCENE_MAIN_H