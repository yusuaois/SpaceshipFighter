#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <iostream>

int main(int, char **) {

  std::cout << "Hello World!";
  // SDL初始化
  if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
    std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  // 创建窗口
  SDL_Window *window = SDL_CreateWindow("SDL3", 800, 600, 0);

  // 创建渲染器
  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

  // 渲染循环
  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        break;
      }
    }

    // 清屏
    SDL_RenderClear(renderer);

    //画一个长方形
    SDL_FRect rect = {100, 100, 200, 200};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // 更新屏幕
    SDL_RenderPresent(renderer);
  }

  // 清理
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}