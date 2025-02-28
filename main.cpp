#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <iostream>

int main(int, char **) {

  std::cout << "Hello World!";
  // SDL初始化
  if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
    std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  //创建窗口
  SDL_Window *window = SDL_CreateWindow("SDL3",800,600, 0);

  //创建渲染器
  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

  //SDL3_Image不需要初始化，会自动初始化

  //清理
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}