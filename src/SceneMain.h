#ifndef SCENE_MAIN_H
#define SCENE_MAIN_H

#include "Object.h"
#include "Scene.h"
#include <list>

// 声明一个名为 Game 的类，但不提供其定义
// 这种声明通常用于提前告知编译器该类的存在，以便在其他地方使用该类类型的变量或参数
class Game;

class SceneMain : public Scene {
public:
  SceneMain();
  ~SceneMain();

  void update(float deltaTime) override;       // Update the scene
  void render() override;                      // Render the scene
  void handleEvent(SDL_Event *event) override; // Handle events
  void init() override;  // Called when the scene is entered
  void clean() override; // Called when the scene is exited

  void keyBoardControl(float deltaTime);
  void shootPlayer();
  void updateProjectiles(float deltaTime);
  void renderProjectiles();

private:
  Game &game;    // The game object
  Player player; // The player object
  // 物体模板
  ProjectilePlayer projectilePlayerTemplate;

  std::list<ProjectilePlayer *>
      ProjectilesPlayer; // The list of player projectiles
};

#endif // SCENE_MAIN_H