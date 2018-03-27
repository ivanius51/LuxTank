#include "stdafx.h"
#include <chrono>
#include <thread>
#include <iostream>

#include <windows.h>

#include "Game.h"
#include "GlobalVariables.h"

void gameLoop()
{
  while (Game::instance().isRunning())
  {
    int startTime = GetTickCount();

    Game::instance().readInput();
    Game::instance().useInput();

    if (!Game::instance().isPaused())
      Game::instance().update();
    Game::instance().draw();

    int elapsedTime = GetTickCount() - startTime;
    if (elapsedTime <= 0)
      elapsedTime = 1;
    double fps = 1000 / elapsedTime;
    if (Game::instance().getFrameDelay() > elapsedTime)
      //Sleep(Game::instance().getFrameDelay() - elapsedTime);//windows
      std::this_thread::sleep_for(std::chrono::milliseconds(Game::instance().getFrameDelay() - elapsedTime));
  }
}

int main()
{
  //can edit settings and load it from file
  Game::instance().initialization(GetConsoleWindow(), false, MAP_SIZE, TILE_SIZE, MAX_FPS);

  gameLoop();
  
  Game::instance().free();
  return 0;
}

