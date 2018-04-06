#include "stdafx.h"
#include <chrono>
#include <thread>
#include <iostream>

#include "Game.h"
#include "GlobalVariables.h"
#include "LuxTank.h"

void gameLoop()
{
  INT64 lastTime = getTimeMks() - 1000;//GetTickCount();
  while (Game::instance().isRunning())
  {
    INT64 startTime = getTimeMks();//GetTickCount();
    INT64 elapsedTime = startTime - lastTime;

    Game::instance().clearInput();
    Game::instance().readInput();

    if (!Game::instance().isPaused())
    {
      if (!Game::instance().isGameOver())
      {
        Game::instance().useInput();
        Game::instance().update((elapsedTime / (double)MSEC_IN_SEC));
        Game::instance().renderFrame();
      }
      else
      {
        int msgResult = MessageBox(Game::instance().getHandle(), "Maybe, want play again?", "Restarting game", MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION);
        if (msgResult == IDYES)
          Game::instance().restartGame();
        else
          Game::instance().pause();
      }
    }
    
    Game::instance().showResult();

    Game::instance().draw();

    lastTime = startTime;
    INT64 elapsed = getTimeMks(startTime);//(GetTickCount() - startTime);
    double fps = (MKS_IN_SEC / (double)elapsed);
    if (Game::instance().getFrameDelay() > elapsed)
    {
      DWORD pauseTime = ((Game::instance().getFrameDelay() - elapsed) / MSEC_IN_SEC) + 1;
      Sleep(pauseTime);//windows
      //std::this_thread::sleep_for(std::chrono::microseconds(Game::instance().getFrameDelay() - elapsed));
    }  
  }
}

int main()
{
  //randomization
  srand(GetTickCount());
  //can edit settings and load it from file
  Game::instance().initialization(GetConsoleWindow(), false, MAP_SIZE, TILE_SIZE, MAX_FPS);
  
  Game::instance().restartGame();
  //Game::instance().startGame();
  gameLoop();

  Game::instance().free();
  return 0;
}

