#include "SceneEnd.h"
#include "Game.h"
#include "SceneMain.h"
#include <SDL.h>
#include <string>

void SceneEnd::update(float deltaTime) {
  if ((blinkTimer -= deltaTime) <= 0) {
    blinkTimer += 1.0f;
  }
};
void SceneEnd::init() {
  if (!SDL_IsTextInputActive()) {
    SDL_StartTextInput();
  }
  if (!SDL_IsTextInputActive()) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to start text input: %s",
                 SDL_GetError());
  }
};

void SceneEnd::render() {
  if (isTyping) {
    renderPhase1();
  } else {
    renderPhase2();
  }
};
void SceneEnd::clean() {};

void SceneEnd::handleEvent(SDL_Event *event) {
  if (isTyping) {
    if (event->type == SDL_TEXTINPUT) {
      name += event->text.text;
    }
    if (event->type == SDL_KEYDOWN) {
      if (event->key.keysym.scancode == SDL_SCANCODE_RETURN) {
        isTyping = false;
        SDL_StopTextInput();
        if (name == "") {
          name = "无名氏";
        }
        game.insertScore(game.getFinalScore(), name);
      }
      if (event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
        removeLastUtf8Char(name);
      }
    }

  } else {
    if (event->type == SDL_KEYDOWN) {
      if (event->key.keysym.scancode == SDL_SCANCODE_J) {
        Scene *scene = new SceneMain();
        game.changeScene(scene);
      }
    }
  }
};

void SceneEnd::renderPhase1() {
  // 渲染游戏结束画面

  // 提示信息
  auto score = game.getFinalScore();
  std::string scoreText = "你的得分是：" + std::to_string(score);
  game.renderTextCentered(scoreText, 0.1, false);
  std::string gameOver = "Game Over";
  game.renderTextCentered(gameOver, 0.4, true);
  std::string instructionText = "请输入你的名字，按回车键确认：";
  game.renderTextCentered(instructionText, 0.6, false);

  // 渲染输入框
  if (name != "") {
    SDL_Point p = game.renderTextCentered(name, 0.8, false);
    if (blinkTimer <= 0.5) {
      game.renderTextPos("_", p.x, p.y);
    }

  } else {
    if (blinkTimer <= 0.5) {
      game.renderTextCentered("_", 0.8, false);
    }
  }
}

void SceneEnd::renderPhase2() {
  // 渲染得分榜
  game.renderTextCentered("得分榜", 0.05, true);
  float posY = 0.2 * game.getWindowHeight();
  int rank = 1;
  for (auto item : game.getScoreBoard()) {
    // 姓名
    game.renderTextPos(std::to_string(rank) + ". " + item.second, 100, posY);
    // 分数
    game.renderTextPos(std::to_string(item.first), 100, posY, false);

    rank++;
    posY += 0.06 * game.getWindowHeight();
  }

  // 重新开始游戏
  if (blinkTimer <= 0.5) {
    game.renderTextCentered("按 J 键重新开始游戏", 0.85, false);
  }
}

void SceneEnd::removeLastUtf8Char(std::string &str) {
  if (str.empty())
    return;
  auto lastChar = str.back();
  if ((lastChar & 0b10000000) == 0b10000000) { // 是否中文字符
    // 0b开头表示二进制数
    str.pop_back();
    while ((str.back() & 0b11000000) !=
           0b11000000) { // 判断是否是中文字符的第一个字节
      str.pop_back();
    }
  }
  str.pop_back();
}