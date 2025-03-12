#include "Game.h"
#include "SceneMain.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <cstddef>

Game::Game() {}

Game::~Game() { clean(); }

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

  // Initialize game scenes
  curScene = new SceneMain();
  curScene->init();

  // Initialize ttf

  // Initialize Background
  nearStars.texture = IMG_LoadTexture(renderer, "assets/image/Stars-A.png");
  SDL_QueryTexture(nearStars.texture, NULL, NULL, &nearStars.width,
                   &nearStars.height);
  nearStars.width/=2;
  nearStars.height/=2;

  farStars.texture = IMG_LoadTexture(renderer, "assets/image/Stars-B.png");
  SDL_QueryTexture(farStars.texture, NULL, NULL, &farStars.width,
                   &farStars.height);
  farStars.speed = 20;
  farStars.width/=2;
  farStars.height/=2;

  // Initialize game scenes
  curScene = new SceneMain();
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

  // 清理SDL_image库
  IMG_Quit();

  // 清理SDL_mixer库
  Mix_CloseAudio();
  Mix_Quit();

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
  
  for (int posY = static_cast<int>(farStars.offset); posY < getWindowHeight(); posY += farStars.height) {
    for (int posX = 0; posX < getWindowWidth(); posX += farStars.width) {
      SDL_Rect dstRect = {posX, posY, farStars.width, farStars.height};
      SDL_RenderCopy(renderer, farStars.texture, nullptr, &dstRect);
    }
  }
  // 近处
  
  for (int posY = static_cast<int>(nearStars.offset); posY < getWindowHeight(); posY += nearStars.height) {
    for (int posX = 0; posX < getWindowWidth(); posX += nearStars.width) {
      SDL_Rect dstRect = {posX, posY, nearStars.width, nearStars.height};
      SDL_RenderCopy(renderer, nearStars.texture, nullptr, &dstRect);
    }
  }
}