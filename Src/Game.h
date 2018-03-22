#pragma once
#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include <map>

struct POINTComarator {
  bool operator() (const POINT& point1, const POINT& point2) const
  {
    if (point1.x != point2.x)
      return point1.x < point2.x;
    else
      return point1.y < point2.y;
  }
};

#include "GameObject.h"
#include "Command.h"

class Game
{
public:
  static Game& instance();

  void initialization(HWND handle, UINT mapSize = 15, UINT tileSize = 32, UINT fpsmax = 60);
  void free();

  Command* input();
  void update();
  void draw();

  Tank* getPlayer();
  Gold* getGold();
  GameObject* getObject(int x, int y);
  GameObject* getObject(POINT point);
  bool isWalkable(int x, int y);
  bool isWalkable(POINT point);

  void increaseScore();

  UINT getFrameDelay();

  UINT getWindowSize();
  UINT getTileSize();
  UINT getMapSize();
  UINT getTextHeightPx();

  HDC getBufferDc();
  HBITMAP getBuffer();
  HDC getStaticLayerDc();
  HBITMAP getStaticLayer();
  void drawBitmap(HDC hdc, int x, int y, HBITMAP hBitmap, bool transparent = false);
  void drawBitmap(int x, int y, HBITMAP hBitmap, bool transparent = false);
protected:
private:
  //Singlton
  Game(Game const&) = delete;             
  Game(Game&&) = delete;                  
  Game& operator=(Game const&) = delete;  
  Game& operator=(Game &&) = delete;      
  Game();
  ~Game();
  
  HDC hdc_ = nullptr;
  HWND handle_ = nullptr;
  HDC textLayerDc_ = nullptr;
  HDC staticLayerDc_ = nullptr;
  HDC bufferDc_ = nullptr;
  HBITMAP textLayer_ = nullptr;
  HBITMAP staticLayer_ = nullptr;
  HBITMAP buffer_ = nullptr;

  UINT frameDelay_ = 0;
  UINT tileSize_ = 32;
  UINT mapSize_ = 0;
  UINT windowSize_ = 0;
  UINT textHeightPx_ = 0;
  UINT startTime_ = 0;

  UINT score_ = 0;
  Tank* player_ = nullptr;
  Gold* gold_ = nullptr;
  std::map<POINT, GameObject*, POINTComarator> tiles_;
  
  void generateNewMap();

  void renderObjects();
  void drawObjects();
  void drawGui();
};

#endif // GAME_H