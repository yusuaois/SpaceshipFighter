#include "Game.h"
#include <SDL_events.h>

Game::Game(){

}

Game::~Game(){

}

void Game::run(){
    while(isRunning){
        SDL_Event event;
        curScene->handleEvent(&event);
        curScene->update();
        curScene->render();
    }
}

void Game::init(){
    
}

void Game::clean(){

}

void Game::changeScene(Scene* scene){
    if(curScene != nullptr){
        curScene->clean();
        delete curScene;
    }
    curScene = scene;
    curScene->init();
}