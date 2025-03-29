#ifndef GAME_H
#define GAME_H

#include "Object.h"
#include "Scene.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
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
  SDL_Point renderTextCentered(std::string text, float posY, bool isTitle);
  void renderTextPos(std::string text, int posX, int posY);

  // setters
  void setFinalScore(int score) { finalScore = score; }

  // getter
  SDL_Window *getWindow() { return window; }
  SDL_Renderer *getRenderer() { return renderer; }
  int getWindowWidth() { return windowWidth; }
  int getWindowHeight() { return windowHeight; }
  int getFinalScore() { return finalScore; }

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
  int windowWidth = 600;
  int windowHeight = 800;
  int FPS = 60;
  Uint32 frameTime;
  float deltaTime;
  int finalScore = 0;

  Background nearStars;
  Background farStars;

  void backgroundUpdate(float deltaTime);
  void renderBackground();
};

#endif