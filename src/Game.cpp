#include "Game.h"
#include "SceneMain.h"
#include "SceneTitle.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_log.h>
#include <SDL_mixer.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_scancode.h>
#include <SDL_surface.h>
#include <SDL_ttf.h>
#include <SDL_video.h>
#include <cstddef>
#include <fstream>

Game::Game() {}

Game::~Game() {
  saveScore();
  clean();
}

// 定义Game类的run方法，用于运行游戏的主循环
void Game::run() {
  while (isRunning) {
    auto frameStart = SDL_GetTicks();
    SDL_Event event;
    handleEvent(&event);
    update(deltaTime);
    render();
    auto frameDelay = SDL_GetTicks() - frameStart;
    if (frameDelay < frameTime) {
      SDL_Delay(frameTime - frameDelay);
      deltaTime = frameTime / 1000.0f;
    } else {
      deltaTime = frameDelay / 1000.0f;
    }
  }
}

void Game::init() {
  frameTime = 1000 / FPS;
  // Initialize SDL
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL: %s\n",
                 SDL_GetError());
    isRunning = false;
  }

  // Initialize window
  window = SDL_CreateWindow("SDL Game", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight,
                            SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to create window: %s\n",
                 SDL_GetError());
    isRunning = false;
  }

  // Initialize renderer
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to create renderer: %s\n",
                 SDL_GetError());
    isRunning = false;
  }

  // set logical resolution
  SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);

  // Initialize text
  if (TTF_Init() != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL_ttf: %s\n",
                 TTF_GetError());
    isRunning = false;
  }

  // Initialize audio
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL_mixer: %s\n",
                 Mix_GetError());
    isRunning = false;
  }

  // Initialize image
  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL_image: %s\n",
                 IMG_GetError());
    isRunning = false;
  }

  // Initialize mixer
  if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) != (MIX_INIT_MP3 | MIX_INIT_OGG)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL_mixer: %s\n",
                 Mix_GetError());
    isRunning = false;
  }

  // Initialize audio
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to open audio: %s\n",
                 Mix_GetError());
    isRunning = false;
  }

  Mix_AllocateChannels(32);

  // volume
  Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
  Mix_Volume(-1, MIX_MAX_VOLUME / 8);

  // Initialize font
  if (TTF_Init() == -1) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL_ttf: %s\n",
                 TTF_GetError());
    isRunning = false;
  }

  // Initialize Background
  nearStars.texture = IMG_LoadTexture(renderer, "assets/image/Stars-A.png");
  if (nearStars.texture == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to load texture: %s\n",
                 IMG_GetError());
    isRunning = false;
  }
  SDL_QueryTexture(nearStars.texture, NULL, NULL, &nearStars.width,
                   &nearStars.height);
  nearStars.width /= 2;
  nearStars.height /= 2;

  farStars.texture = IMG_LoadTexture(renderer, "assets/image/Stars-B.png");
  if (farStars.texture == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to load texture: %s\n",
                 IMG_GetError());
    isRunning = false;
  }
  SDL_QueryTexture(farStars.texture, NULL, NULL, &farStars.width,
                   &farStars.height);
  farStars.speed = 20;
  farStars.width /= 2;
  farStars.height /= 2;

  // 载入字体
  titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 64);
  textFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 32);
  if (titleFont == nullptr || textFont == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to load font: %s\n",
                 TTF_GetError());
    isRunning = false;
  }

  // 载入得分
  loadScore();

  // Initialize game scenes
  curScene = new SceneTitle();
  curScene->init();
}

// 定义Game类的clean成员函数，用于清理游戏资源
void Game::clean() {
  if (curScene != nullptr) {
    curScene->clean();
    delete curScene;
  }
  if (nearStars.texture != nullptr) {
    SDL_DestroyTexture(nearStars.texture);
  }
  if (farStars.texture != nullptr) {
    SDL_DestroyTexture(farStars.texture);
  }
  if (textFont != nullptr) {
    TTF_CloseFont(textFont);
  }
  if (titleFont != nullptr) {
    TTF_CloseFont(titleFont);
  }

  // 清理SDL_image库
  IMG_Quit();

  // 清理SDL_mixer库
  Mix_CloseAudio();
  Mix_Quit();

  // 清理SDL_ttf库
  TTF_Quit();

  // 销毁渲染器，释放相关资源
  SDL_DestroyRenderer(renderer);
  // 销毁窗口，释放相关资源
  SDL_DestroyWindow(window);
  // 退出SDL库，释放所有相关资源
  SDL_Quit();
}

// 定义Game类的成员函数changeScene，用于切换游戏场景
void Game::changeScene(Scene *scene) {
  // 检查当前场景是否不为空
  if (curScene != nullptr) {
    // 调用当前场景的clean函数，进行清理工作，如释放资源等
    curScene->clean();
    // 删除当前场景对象，释放内存
    delete curScene;
  }
  // 将传入的新场景指针赋值给当前场景指针
  curScene = scene;
  // 调用新场景的init函数，进行初始化工作，如加载资源等
  curScene->init();
}

void Game::handleEvent(SDL_Event *event) {

  while (SDL_PollEvent(event)) {
    if (event->type == SDL_QUIT) {
      isRunning = false;
    }
    if (event->type == SDL_KEYDOWN) {
      if (event->key.keysym.sym == SDL_SCANCODE_F11) {
        isFullScreen = !isFullScreen;
        if (isFullScreen)
          SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        else {
          SDL_SetWindowFullscreen(window, 0);
        }
      }
    }
    curScene->handleEvent(event);
  }
}

void Game::update(float deltaTime) {
  backgroundUpdate(deltaTime);
  curScene->update(deltaTime);
}

void Game::render() {
  // 清空
  SDL_RenderClear(renderer);

  // 渲染背景
  renderBackground();

  curScene->render();

  // 显示更新
  SDL_RenderPresent(renderer);
}

SDL_Point Game::renderTextCentered(std::string text, float posY, bool isTitle) {

  SDL_Color color = {255, 255, 255, 255};
  SDL_Surface *surface;
  if (isTitle) {
    surface = TTF_RenderUTF8_Solid(titleFont, text.c_str(), color);
  } else {
    surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  int y = static_cast<int>((getWindowHeight() - surface->h) * posY);
  SDL_Rect rect = {getWindowWidth() / 2 - surface->w / 2, y, surface->w,
                   surface->h};
  SDL_RenderCopy(renderer, texture, NULL, &rect);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);

  return {rect.x + rect.w, rect.y};
}

void Game::backgroundUpdate(float deltaTime) {
  nearStars.offset += nearStars.speed * deltaTime;
  if (nearStars.offset >= 0) {
    nearStars.offset -= nearStars.height;
  }

  farStars.offset += farStars.speed * deltaTime;
  if (farStars.offset >= 0) {
    farStars.offset -= farStars.height;
  }
}

void Game::renderBackground() {

  // 远处

  for (int posY = static_cast<int>(farStars.offset); posY < getWindowHeight();
       posY += farStars.height) {
    for (int posX = 0; posX < getWindowWidth(); posX += farStars.width) {
      SDL_Rect dstRect = {posX, posY, farStars.width, farStars.height};
      SDL_RenderCopy(renderer, farStars.texture, nullptr, &dstRect);
    }
  }
  // 近处

  for (int posY = static_cast<int>(nearStars.offset); posY < getWindowHeight();
       posY += nearStars.height) {
    for (int posX = 0; posX < getWindowWidth(); posX += nearStars.width) {
      SDL_Rect dstRect = {posX, posY, nearStars.width, nearStars.height};
      SDL_RenderCopy(renderer, nearStars.texture, nullptr, &dstRect);
    }
  }
}

void Game::renderTextPos(std::string text, int posX, int posY, bool isLeft) {
  SDL_Color color = {255, 255, 255, 255};
  SDL_Surface *surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_Rect dstRect;
  if (isLeft) {
    dstRect = {posX, posY, surface->w, surface->h};
  } else {
    dstRect = {getWindowWidth() - posX - surface->w, posY, surface->w,
               surface->h};
  }
  SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
}

void Game::insertScore(int score, std::string name) {
  scoreBoard.insert({score, name});
  if (scoreBoard.size() > 8) {
    scoreBoard.erase(--scoreBoard.end());
  }
}

void Game::saveScore() {
  // 保存得分榜
  std::ofstream file("assets/save.dat");
  if (!file.is_open()) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open save file");
    return;
  }
  for (auto it = scoreBoard.begin(); it != scoreBoard.end(); ++it) {
    file << it->first << " " << it->second << std::endl;
  }
}

void Game::loadScore() {
  // 加载得分榜
  std::ifstream file("assets/save.dat");
  if (!file.is_open()) {
    SDL_Log("Failed to open save file");
    return;
  }
  scoreBoard.clear();
  int score;
  std::string name;
  while (file >> score >> name) {
    scoreBoard.insert({score, name});
  }
}