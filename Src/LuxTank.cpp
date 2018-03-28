#include "stdafx.h"
#include <chrono>
#include <thread>
#include <iostream>

#include <windows.h>

#include "Game.h"
#include "GlobalVariables.h"

void gameLoop()
{
  INT64 lastTime = getTimeMks() - 1000;//GetTickCount();
  while (Game::instance().isRunning())
  {
    INT64 startTime = getTimeMks();//GetTickCount();
    INT64 elapsedTime = startTime - lastTime;

    Game::instance().readInput();
    Game::instance().useInput();

    if (!Game::instance().isPaused())
    {
      Game::instance().update((elapsedTime / (double)MSEC_IN_SEC));
      Game::instance().renderFrame();
      Game::instance().draw();
    };
    if (Game::instance().isGameOver())
      Game::instance().showResult();

    lastTime = startTime;
    INT64 elapsed = getTimeMks(startTime);//(GetTickCount() - startTime);
    double fps = (MKS_IN_SEC / (double)elapsed);
    if (Game::instance().getFrameDelay() > elapsed)
      //Sleep(Game::instance().getFrameDelay() - elapsedTime);//windows
      std::this_thread::sleep_for(std::chrono::microseconds(Game::instance().getFrameDelay() - elapsed));
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

