#ifndef SCENE_MAIN_H
#define SCENE_MAIN_H

#include "Object.h"
#include "Scene.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <list>
#include <map>
#include <random>

// 声明一个名为 Game 的类，但不提供其定义
// 这种声明通常用于提前告知编译器该类的存在，以便在其他地方使用该类类型的变量或参数
class Game;

class SceneMain : public Scene {
public:
  ~SceneMain();

  void update(float deltaTime) override;       // Update the scene
  void render() override;                      // Render the scene
  void handleEvent(SDL_Event *event) override; // Handle events
  void init() override;  // Called when the scene is entered
  void clean() override; // Called when the scene is exited

private:
  bool isDead = false;                       // The game object
  Player player;                             // The player object
  std::mt19937 gen;                          // Random number generator
  std::uniform_real_distribution<float> dis; // Random number distribution
  Mix_Music *bgm;                            // Background music
  SDL_Texture *uiHealth;                     // UI health texture
  TTF_Font *scoreFont;                       // Score font
  int score = 0;                             // Score
  float timerEnd = 0.0f;                     // Timer for end of game

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
      ProjectilesEnemies;                    // The list of enemy projectiles
  std::list<Explosion *> Explosions;         // The list of explosions
  std::list<Item *> Items;                   // The list of items
  std::map<std::string, Mix_Chunk *> sounds; // The list of sounds

  // 渲染
  void renderProjectiles();
  void renderEnemies();
  void renderItems();
  void renderUI();
  void renderEnemyProjectiles();
  void renderExplosions();

  // 更新
  void spawnEnemies();
  void updateEnemies(float deltaTime);
  void updateProjectiles(float deltaTime);
  void updateItems(float deltaTime);
  void updatePlayer(float deltaTime);
  void updateExplosions(float deltaTime);
  void updateEnemyProjectiles(float deltaTime);
  void keyBoardControl(float deltaTime);
  void changeSceneDelayed(float deltaTime,float delay);

  // 其他
  void dropItem(Enemy *enemy);
  void playerGetItem(Item *item);
  void shootPlayer();
  void shootEnemy(Enemy *enemy);
  SDL_FPoint getDirectionVector(Enemy *enemy);
  void enemyExplode(Enemy *enemy);
  
};

#endif // SCENE_MAIN_H