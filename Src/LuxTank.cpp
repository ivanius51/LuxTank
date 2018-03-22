// LuxTank.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
//#include <chrono>
//#include <thread>
#include <iostream>

#include <windows.h>

#include "Game.h"

int main()
{
  //can edit settings and load it from file
  Game::instance().initialization(GetConsoleWindow(), 15, 32, 60);

  while (true)
  {
    int startTime = GetTickCount();

    Command* UserInputCommand = Game::instance().input();
    if (UserInputCommand)
      UserInputCommand->execute();
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

