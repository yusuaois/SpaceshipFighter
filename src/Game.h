#ifndef GAME_H
#define GAME_H

#include "Object.h"
#include "Scene.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <map>
#include <string>

// 单例模式：全局只有一个实例
// 全局只有一个实例，并且只能通过instance()方法获取
// 私有构造函数，防止外部创建实例
// 私有析构函数，防止外部删除实例

class Game {
public:
  // 单例函数
  static Game &GetInstance() {
    static Game instance;
    return instance;
  };

  ~Game();
  void run();
  void init();
  void clean();
  void changeScene(Scene *scene);

  void handleEvent(SDL_Event *event);
  void update(float deltaTime);
  void render();

  // 渲染文字函数
  SDL_FPoint renderTextCentered(std::string text, float posY, bool isTitle);
  void renderTextPos(std::string text, int posX, int posY, bool isLeft = true);

  // setters
  void setFinalScore(int score) { finalScore = score; }
  void insertScore(int score, std::string name);

  // getter
  SDL_Window *getWindow() { return window; }
  SDL_Renderer *getRenderer() { return renderer; }
  float getWindowWidth() { return windowWidth; }
  float getWindowHeight() { return windowHeight; }
  int getFinalScore() { return finalScore; }
  std::multimap<int, std::string, std::greater<int>> &getScoreBoard() {
    return scoreBoard;
  }

private:
  Game();
  // 声明一个拷贝构造函数并使用 delete 关键字禁用它
  // 这意味着禁止通过拷贝构造函数来创建类的对象
  // 例如，禁止使用如下语法：Game newGame = existingGame;
  Game(const Game &) = delete;
  // 声明一个赋值运算符重载函数，并使用 delete 关键字禁用该函数
  // 这意味着禁止对该类的对象进行赋值操作

  TTF_Font *titleFont;
  TTF_Font *textFont;

  bool isRunning = true;
  Scene *curScene = nullptr;
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  float windowWidth = 600;
  float windowHeight = 800;
  int FPS = 60;
  Uint32 frameTime;
  float deltaTime;
  int finalScore = 0;
  bool isFullScreen = false;

  Background nearStars;
  Background farStars;

  // multimap 用于存储分数, 键为分数，值为玩家名,
  // 按分数从大到小排序(std::greater<int>)
  std::multimap<int, std::string, std::greater<int>> scoreBoard;

  void backgroundUpdate(float deltaTime);
  void renderBackground();
  void saveScore();
  void loadScore();
};

#endif