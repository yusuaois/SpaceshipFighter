#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_mutex.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#include <SDL_ttf.h>
#include <iostream>

int main(int, char **) {
  std::cout << "Hello, World!" << std::endl;
  // SDL初始化
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }
  // 创建窗口
  SDL_Window *window =
      SDL_CreateWindow("Hello World!", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
  // 创建渲染器
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  // IMG初始化
  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != (IMG_INIT_PNG | IMG_INIT_JPG)) {
    std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
    return 1;
  }

  // 加载图片
  SDL_Texture *texture = IMG_LoadTexture(renderer, "assets/image/bg.png");

  // 音频初始化
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
    return 1;
  }

  // 读取音频文件
  Mix_Music *music =
      Mix_LoadMUS("assets/music/03_Racing_Through_Asteroids_Loop.ogg");

  // 播放音频文件
  Mix_PlayMusic(music, -1);

  // ttf初始化
  if (TTF_Init() < 0) {
    std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
    return 1;
  }

  // 加载字体
  TTF_Font *font = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 24);

  // 创建文本纹理
  SDL_Color color = {255, 255, 255, 255};
  SDL_Surface *surface = TTF_RenderUTF8_Solid(font, "中文Hello, World!", color);
  SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, surface);

  // Do something with the window and renderer here...
  // 渲染循环
  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        break;
      }
    }

    // 清屏
    SDL_RenderClear(renderer);
    // 画一个长方形
    SDL_Rect rect = {100, 100, 200, 200};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);

    // 画图片
    SDL_Rect dstrect = {200, 200, 200,  200};
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);

    // 画文本
    SDL_Rect textRect = {300, 300, surface->w, surface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // 更新屏幕
    SDL_RenderPresent(renderer);
  }

  // 释放图片资源
  SDL_DestroyTexture(texture);
  IMG_Quit();

  // 释放音频资源
  Mix_FreeMusic(music);
  Mix_CloseAudio();
  Mix_Quit();

  // 释放字体资源
  SDL_DestroyTexture(textTexture);
  TTF_CloseFont(font);
  TTF_Quit();

  // 清理并退出
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}