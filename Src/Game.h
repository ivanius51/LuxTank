//#pragma once
#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include <map>
#include <memory>

#include "GameObject.h"
#include "Command.h"

class Game
{
public:
  static Game& instance();
  bool isRunning = true;

  void initialization(HWND handle, UINT mapSize = MAP_SIZE, UINT tileSize = TILE_SIZE, UINT fpsmax = MAX_FPS);
  void free();
  //main
  Command* input() const;
  void update();
  void draw();
  void test();
  //world
  Tank* getPlayer();
  Gold* getGold();
  GameObject* getObject(int x, int y);
  GameObject* getObject(POINT point);
  void deleteObject(GameObject* gameobject);
  bool isWalkable(int x, int y);
  bool isWalkable(POINT point);
  bool isValidPosition(POINT point);
  bool isValidPosition(int x, int y);
  bool canMoveTo(POINT point);
  bool canMoveTo(int x, int y);
  bool isInVisibleDistance(POINT first, POINT second);
  bool isIntersection(POINT first, POINT second);
  //bool checkCollision(int x, int y);
  //bool checkCollision(POINT point);
  GameObject* collidedWith(GameObject* gameobject);
  UINT getTileSize();
  UINT getMapSize();
  void addBullet(std::shared_ptr<Bullet> bullet);
  //
  void increaseScore();
  //
  UINT getFrameDelay();
  UINT getWindowSize();
  UINT getTextHeightPx();
  //graphics
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
  HBITMAP textLayer_ = nullptr;
  HDC staticLayerDc_ = nullptr;
  HBITMAP staticLayer_ = nullptr;
  HDC bufferDc_ = nullptr;
  HBITMAP buffer_ = nullptr;

  UINT frameDelay_ = 0;
  UINT tileSize_ = 0;
  UINT mapSize_ = 0;
  UINT windowSize_ = 0;
  UINT borderSize_ = 0;
  UINT textHeightPx_ = 0;
  UINT startTime_ = 0;

  UINT score_ = 0;
  Tank* player_ = nullptr;
  Gold* gold_ = nullptr;
  std::map<POINT, GameObject*, POINTComarator> tiles_;
  std::vector<std::shared_ptr<Bullet>> bullets_;

  void generateNewMap();

  void drawBorder();
  void renderObjects();
  void drawObjects();
  void drawGui();
};

#endif // GAME_H