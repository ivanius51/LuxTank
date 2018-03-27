//#pragma once
#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include <memory>

#include "GameObject.h"
#include "Command.h"

#include "World.h"

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
  void startGame();
  void stopGame();
  void pause();
  void resume();
  bool isRunning();
  bool isPaused();
  void increaseScore();
  void showResult();
  //
  World getWorld();
  void addBullet(std::shared_ptr<Bullet> bullet);
  void addBullet(Bullet* bullet);
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

  UINT avgFrameTime = 0;
  UINT frameTime = 0;
  UINT frameCounter = 0;
protected:
private:
  //Singlton
  Game(Game const&) = delete;
  Game(Game&&) = delete;
  Game& operator=(Game const&) = delete;
  Game& operator=(Game &&) = delete;
  Game();
  ~Game();
  //
  World world_;
  //graphic
  HDC hdc_ = nullptr;
  HWND handle_ = nullptr;
  HDC textLayerDc_ = nullptr;
  HBITMAP textLayer_ = nullptr;
  HDC staticLayerDc_ = nullptr;
  HBITMAP staticLayer_ = nullptr;
  HDC bufferDc_ = nullptr;
  HBITMAP buffer_ = nullptr;
  UINT frameDelay_ = 0;
  UINT windowSize_ = 0;
  UINT borderSize_ = 0;
  UINT textHeightPx_ = 0;
  //game
  UINT startTime_ = 0;
  bool isRunning_ = true;
  bool isPaused_ = true;
  UINT score_ = 0;
  //input
  std::vector<std::unique_ptr<Command>> inputs_;
  //bullets
  std::vector<std::shared_ptr<Bullet>> bullets_;

  std::string gameResult_ = "Paused";

  void drawBorder();
  void renderObjects();
  void drawObjects();
  void drawGui();
};

#endif // GAME_H