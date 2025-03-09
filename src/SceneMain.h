#ifndef SCENE_MAIN_H
#define SCENE_MAIN_H

#include "Object.h"
#include "Scene.h"
#include <SDL_rect.h>
#include <list>
#include <random>

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
  void spawnEnemies();
  void updateEnemies(float deltaTime);
  void renderEnemies();
  void shootEnemy(Enemy *enemy);
  SDL_FPoint getDirectionVector(Enemy *enemy);
  void updateEnemyProjectiles(float deltaTime);
  void renderEnemyProjectiles();
  void enemyExplode(Enemy *enemy);
  void updatePlayer(float deltaTime);
  void updateExplosions(float deltaTime);
  void renderExplosions();
  void dropItem(Enemy *enemy);
  void updateItems(float deltaTime);
  void playerGetItem(Item *item);
  void renderItems();

private:
  bool isDead = false;
  Game &game;                                // The game object
  Player player;                             // The player object
  std::mt19937 gen;                          // Random number generator
  std::uniform_real_distribution<float> dis; // Random number distribution

  // 物体模板
  ProjectilePlayer projectilePlayerTemplate;
  Enemy enemyTemplate;
  ProjectileEnemy projectileEnemyTemplate;
  Explosion explosionTemplate;
  Item itemLifeTemplate;
  Item itemShieldTemplate;
  Item itemTimeTemplate;

  // 物体列表
  std::list<ProjectilePlayer *>
      ProjectilesPlayer;      // The list of player projectiles
  std::list<Enemy *> Enemies; // The list of enemies
  std::list<ProjectileEnemy *>
      ProjectilesEnemies;            // The list of enemy projectiles
  std::list<Explosion *> Explosions; // The list of explosions
  std::list<Item *> Items;           // The list of items
};

#endif // SCENE_MAIN_H