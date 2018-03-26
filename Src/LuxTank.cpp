#include "stdafx.h"
//#include <chrono>
//#include <thread>
#include <iostream>

#include <windows.h>

#include "Game.h"
#include "GlobalVariables.h"

int main()
{
  //can edit settings and load it from file
  Game::instance().initialization(GetConsoleWindow(), false, MAP_SIZE, TILE_SIZE, MAX_FPS);

  while (Game::instance().isRunning())
  {
    int startTime = GetTickCount();
    if (startTime % 5 != 0)
    {
      Command* UserInputCommand = Game::instance().input();
      if (UserInputCommand != nullptr)
        UserInputCommand->execute();
    }

    Game::instance().update();
    Game::instance().draw();

    int elapsedTime = GetTickCount() - startTime;
    if (Game::instance().getFrameDelay() > elapsedTime)
      Sleep(Game::instance().getFrameDelay() - elapsedTime);
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  Game::instance().free();
  return 0;
}

