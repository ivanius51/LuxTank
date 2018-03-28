#include "stdafx.h"
#include <chrono>
#include <thread>
#include <iostream>

#include <windows.h>

#include "Game.h"
#include "GlobalVariables.h"

void gameLoop()
{
  UINT lastTime = GetTickCount();
  while (Game::instance().isRunning())
  {
    UINT startTime = GetTickCount();
    UINT elapsedTime = startTime - lastTime;
    if (elapsedTime <= 0)
      elapsedTime = 1;

    Game::instance().readInput();
    Game::instance().useInput();

    if (!Game::instance().isPaused())
    {
      Game::instance().update(elapsedTime);
      Game::instance().draw();
    }
    else
      Game::instance().showResult();

    lastTime = startTime;
    UINT elapsed = (GetTickCount() - startTime);
    if (elapsed <= 0)
      elapsed = 1;
    double fps = 1000 / elapsed;
    if (Game::instance().getFrameDelay() > elapsed)
      //Sleep(Game::instance().getFrameDelay() - elapsedTime);//windows
      std::this_thread::sleep_for(std::chrono::milliseconds(Game::instance().getFrameDelay() - elapsed));
  }
}

int main()
{
  //randomization
  srand(GetTickCount());
  //can edit settings and load it from file
  Game::instance().initialization(GetConsoleWindow(), false, MAP_SIZE, TILE_SIZE, MAX_FPS);
  
  Game::instance().startGame();
  gameLoop();
  
  Game::instance().free();
  return 0;
}

