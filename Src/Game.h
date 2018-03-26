//#pragma once
#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include <memory>

#include "GameObject.h"
#include "Command.h"

class Game
{
public:
  static Game& instance();

  void initialization(HWND handle, bool topmost, UINT mapSize = MAP_SIZE, UINT tileSize = TILE_SIZE, UINT fpsmax = MAX_FPS);
  void free();
  //main
  void readInput();
  void useInput();
  void clearInput();

  void update();
  void draw();
  void test();
  //
  void stopGame();
  void pause();
  void resume();
  bool isRunning();
  bool isPaused();
  void increaseScore();
  //world/map functions
  GameObject* getPlayer();
  GameObject* getGold();
  GameObject* getObject(int x, int y);
  GameObject* getObject(POINT point);
  bool deleteObject(GameObject* gameobject);
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
  UINT getFrameDelay();
  UINT getWindowSize();
  UINT getTextHeightPx();
  //graphics
  HDC getBufferDc();
  HBITMAP getBuffer();
  HDC getStaticLayerDc();
  HBITMAP getStaticLayer();
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

  bool isRunning_ = true;
  bool isPaused_ = false;
  UINT score_ = 0;
  std::weak_ptr<GameObject> player_;
  std::weak_ptr<GameObject> gold_;
  std::vector<std::shared_ptr<GameObject>> tiles_;
  std::vector<std::shared_ptr<Bullet>> bullets_;
  std::vector<std::shared_ptr<Command>> inputs_;

  void generateNewMap();

  void drawBorder();
  void renderObjects();
  void drawObjects();
  void drawGui();
};

#endif // GAME_H