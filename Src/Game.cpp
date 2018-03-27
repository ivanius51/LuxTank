#include <exception>

#include "stdafx.h"
#include <algorithm>
#include <string>
#include <iostream>
#include <conio.h>

#include <math.h>

#include "GameObject.h"
#include "Game.h"


Game& Game::instance()
{
  static Game* instance = new Game();
  return *instance;
}

void Game::initialization(HWND handle, bool topmost, UINT mapSize, UINT tileSize, UINT fpsmax)
{
  if (((tileSize & (tileSize - 1)) != 0) || tileSize < 16 || tileSize > 256)
    throw std::invalid_argument("tile size must be a power of 2 and hightest that 8 and lower than 256");

  if (handle != INVALID_HANDLE_VALUE)
  {
    handle_ = handle;
    ShowWindow(handle, SW_HIDE);
    RECT Screen, Window, Client;
    hdc_ = GetDC(handle_);
    windowSize_ = mapSize * tileSize;
    borderSize_ = 4;
    textHeightPx_ = 32 - borderSize_ * 2 + windowSize_ % 12;

    world_ = World(mapSize, tileSize);

    if (fpsmax > 500)
      frameDelay_ = 1;
    else
      frameDelay_ = int(1000 / fpsmax);

    //hide cursor of console
    HANDLE consoleOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (consoleOutputHandle == INVALID_HANDLE_VALUE)
      throw std::runtime_error("GetStdHandle error");
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(consoleOutputHandle, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(consoleOutputHandle, &cursorInfo);
    //buffer size of console calculate
    int x = int(windowSize_ / 8) + 1;//8px for border
    int y = int(windowSize_ / 12) + 3;//12px and two line for text
    //resize console
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(consoleOutputHandle, &bufferInfo);
    SMALL_RECT& windowInfo = bufferInfo.srWindow;
    COORD windowSize = { windowInfo.Right - windowInfo.Left + 1, windowInfo.Bottom - windowInfo.Top + 1 };
    if (windowSize.X > x || windowSize.Y > y)
    {
      SMALL_RECT windowinfo = { 0, 0, SHORT(x < windowSize.X ? x - 1 : windowSize.X - 1), SHORT(y < windowSize.Y ? y - 1 : windowSize.Y - 1) };
      SetConsoleWindowInfo(consoleOutputHandle, TRUE, &windowinfo);
    }
    COORD size = { SHORT(x), SHORT(y) };
    SetConsoleScreenBufferSize(consoleOutputHandle, size);
    SMALL_RECT windowinfo = { 0, 0, SHORT(x - 1), SHORT(y - 1) };
    SetConsoleWindowInfo(consoleOutputHandle, TRUE, &windowinfo);

    //HMENU systemMenu = GetSystemMenu(handle, false);
    //DeleteMenu(systemMenu, SC_CLOSE, MF_BYCOMMAND);
    //DeleteMenu(systemMenu, SC_MINIMIZE, MF_BYCOMMAND);
    //DeleteMenu(systemMenu, SC_MAXIMIZE, MF_BYCOMMAND);
    //DeleteMenu(systemMenu, SC_SIZE, MF_BYCOMMAND);

    //prepare window
    system("cls");
    //SetBkMode(hdc_, TRANSPARENT);

    //Create Text Layer DC,Bitmap
    textLayerDc_ = CreateCompatibleDC(hdc_);
    if (!textLayerDc_)
      throw std::runtime_error("can't Create DC textLayer");
    textLayer_ = CreateCompatibleBitmap(hdc_, windowSize_ + borderSize_ * 2, textHeightPx_);
    if (!textLayer_)
      throw std::runtime_error("can't Create Bitmap textLayer");
    HFONT hFont = CreateFont(20, 6, 0, 0, FW_DONTCARE, 0, 0, 0, ANSI_CHARSET,
      OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH,
      "SYSTEM_FIXED_FONT");
    BitBlt(hdc_, 0, textHeightPx_, windowSize_ + borderSize_ * 2, windowSize_ + borderSize_ * 2, hdc_, 0, 0, BLACKNESS);

    bufferDc_ = CreateCompatibleDC(hdc_);
    if (!bufferDc_)
      throw std::runtime_error("can't Create DC Buffer");
    buffer_ = CreateCompatibleBitmap(hdc_, windowSize_, windowSize_);
    if (!buffer_)
      throw std::runtime_error("can't Create Bitmap Buffer");

    staticLayerDc_ = CreateCompatibleDC(hdc_);
    if (!bufferDc_)
      throw std::runtime_error("can't Create DC Buffer");
    staticLayer_ = CreateCompatibleBitmap(hdc_, windowSize_ + borderSize_ * 2, windowSize_ + borderSize_ * 2);
    if (!buffer_)
      throw std::runtime_error("can't Create Bitmap Buffer");

    world_.generateNewMap();

    GetWindowRect(handle, &Window);
    GetWindowRect(GetDesktopWindow(), &Screen);
    GetClientRect(handle, &Client);

    MoveWindow(handle, (Screen.right / 2) - (Window.right - Window.left) / 2, (Screen.bottom / 2) - (Window.bottom - Window.top) / 2, Window.right - Window.left, textHeightPx_ + windowSize_ + borderSize_ * 2, 1);
    SetWindowLong(handle, GWL_STYLE, WS_POPUP);
    SetWindowRgn(handle, CreateRectRgn(Client.left + borderSize_ / 2 + 1, Client.top + borderSize_ / 2, windowSize_ + borderSize_ * 2 + borderSize_ / 2 - 1, textHeightPx_ + windowSize_ + borderSize_ * 2 + 1), TRUE);
    //fullscreen TOPMOST
    if (topmost)
      SetWindowPos(handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    ShowWindow(handle, SW_RESTORE);

    startTime_ = GetTickCount();
  }
  else
    throw std::runtime_error("initialization handle is invalid");
}

void Game::free()
{
  ReleaseDC(handle_, hdc_);
  if (textLayerDc_)
    DeleteDC(textLayerDc_);
  if (textLayer_)
    DeleteObject(textLayer_);
  if (staticLayerDc_)
    DeleteDC(staticLayerDc_);
  if (staticLayer_)
    DeleteObject(staticLayer_);
  if (bufferDc_)
    DeleteDC(bufferDc_);
  if (buffer_)
    DeleteObject(buffer_);
}

void Game::readInput()
{
  Command* command = nullptr;
  if (world_.getPlayer())
  {
    //one commands
    if (_kbhit())
    {
      int symbol = getch();
      //if (symbol == 0 || symbol == 224 || symbol == -32)
      //{
      //  symbol = 0;
      //  getch();
      //  symbol = getch();
      //}
      if ((symbol == KEY_UP) || ((GetAsyncKeyState(VK_UP) & 0x8000) && world_.getPlayer()->getDirection().y == -1))
        command = new RotateCommand(world_.getPlayer(), 0, -1);
      if ((symbol == KEY_DOWN) || ((GetAsyncKeyState(VK_DOWN) & 0x8000) && world_.getPlayer()->getDirection().y == 1))
        command = new RotateCommand(world_.getPlayer(), 0, 1);
      if ((symbol == KEY_LEFT) || ((GetAsyncKeyState(VK_LEFT) & 0x8000) && world_.getPlayer()->getDirection().x == -1))
        command = new RotateCommand(world_.getPlayer(), -1, 0);
      if ((symbol == KEY_RIGHT) || ((GetAsyncKeyState(VK_RIGHT) & 0x8000) && world_.getPlayer()->getDirection().x == 1))
        command = new RotateCommand(world_.getPlayer(), 1, 0);
    }
    if (command)
    {
      command->execute();
      delete command;
      command = nullptr;
      //inputs_.push_back(std::shared_ptr<Command>(command));
    }

    //multiple commands input modifier
    if (GetAsyncKeyState(VK_SPACE) & 0x8001)
      command = new FireCommand(world_.getPlayer());
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8001)
    {
      stopGame();
    }
    if (GetAsyncKeyState(VK_PAUSE) & 0x8001)
    {
      if (isPaused())
        resume();
      else
        pause();
    }

    if (command)
    {
      command->execute();
      delete command;
      command = nullptr;
      //inputs_.push_back(std::unique_ptr<Command>(command));
    }
  }
}

void Game::useInput()
{
  for (auto&& input : inputs_)
  {
    //Command* command = input.release();
    //if (command)
    //{
    //  command->execute();
    //  delete command;
    //  command = nullptr;
    //}
  }
  inputs_.erase(inputs_.begin(), inputs_.end());
}

void Game::clearInput()
{
  inputs_.clear();
}

void Game::addBullet(std::shared_ptr<Bullet> bullet)
{
  bullets_.push_back(bullet);
}

void Game::addBullet(Bullet* bullet)
{
  bullets_.push_back(std::shared_ptr<Bullet>(bullet));
}

void Game::update()
{
  auto tiles = world_.getTiles();
  auto bullets = bullets_;// world_.getBullets();

  //Objects update
  for (auto&& tile : *tiles)
    tile->update();

  for (auto&& bullet : bullets)
    bullet->update();
  //remove collided(stopped) bullets
  bullets.erase(
    std::remove_if(bullets.begin(), bullets.end(),
      [](std::shared_ptr<Bullet> bullet) {return !bullet->isMooving(); }
  ), bullets.end());

  if (world_.getPlayer() && world_.getPlayer()->isDead() || world_.getGold() && world_.getGold()->isDead())
    stopGame();

  //remove Died(destroyed) objects
  tiles->erase(
    std::remove_if(tiles->begin(), tiles->end(),
      [](std::shared_ptr<GameObject> object) {return object->isDead(); }
  ), tiles->end());
}

void Game::draw()
{
  drawBorder();
  renderObjects();
  drawObjects();
  drawGui();
}

void Game::test()
{

}

void Game::stopGame()
{
  isRunning_ = false;
}

void Game::pause()
{
  isPaused_ = true;
}

void Game::resume()
{
  isPaused_ = false;
}

bool Game::isRunning()
{
  return isRunning_;
}

bool Game::isPaused()
{
  return isPaused_;
}

void Game::increaseScore()
{
  score_++;
}

World Game::getWorld()
{
  return world_;
}

UINT Game::getFrameDelay()
{
  return frameDelay_;
}

UINT Game::getWindowSize()
{
  return windowSize_;
}

UINT Game::getTextHeightPx()
{
  return textHeightPx_;
}

HDC Game::getBufferDc()
{
  return bufferDc_;
}

HBITMAP Game::getBuffer()
{
  return buffer_;
}

HDC Game::getStaticLayerDc()
{
  return staticLayerDc_;
}

HBITMAP Game::getStaticLayer()
{
  return staticLayer_;
}

void Game::drawBitmap(int x, int y, HBITMAP hBitmap, bool transparent)
{
  gdi::drawBitmap(hdc_, x, y, hBitmap, transparent);
}

Game::Game()
{
}

Game::~Game()
{
}

void Game::drawBorder()
{
  HGDIOBJ replaced = SelectObject(staticLayerDc_, staticLayer_);

  BitBlt(staticLayerDc_, 0, 0, windowSize_ + borderSize_ * 2, windowSize_ + borderSize_ * 2, staticLayerDc_, 0, 0, BLACKNESS);
  HPEN pen = CreatePen(PS_SOLID, 1, COLOR_WINDOW);
  HGDIOBJ oldPen = SelectObject(staticLayerDc_, pen);
  Rectangle(staticLayerDc_, 1, 1, windowSize_ + borderSize_ * 2 - 1, windowSize_ + borderSize_ * 2 - 1);
  SelectObject(staticLayerDc_, oldPen);
  DeleteObject(pen);
  BitBlt(staticLayerDc_, borderSize_, borderSize_, windowSize_, windowSize_, staticLayerDc_, 0, 0, BLACKNESS);

  SelectObject(staticLayerDc_, replaced);
}

void Game::renderObjects()
{
  auto bullets = bullets_;// *world_.getBullets();
  auto tiles = world_.getTiles();

  HGDIOBJ replacedBuffer = SelectObject(bufferDc_, buffer_);
  HGDIOBJ replacedStatic = SelectObject(staticLayerDc_, staticLayer_);

  //clear buufer
  BitBlt(bufferDc_, 0, 0, windowSize_, windowSize_, bufferDc_, 0, 0, BLACKNESS);
  //std::map<POINT, GameObject*>::const_iterator
  for (auto&& bullet : bullets)
    bullet->draw();
  for (auto object = tiles->begin(); object != tiles->end(); object++)
    object->get()->draw();
  /*
  for (auto&& object : *tiles)
  {
    if (!object->canMove())
      object->draw();
  }
  for (auto&& object : *tiles)
  {
    if (object->canMove())
      object->draw();
  }
  */
  //draw buffer to frame
  BitBlt(staticLayerDc_, borderSize_, borderSize_, windowSize_, windowSize_, bufferDc_, 0, 0, SRCPAINT);

  SelectObject(bufferDc_, replacedBuffer);
  SelectObject(staticLayerDc_, replacedStatic);
}

void Game::drawObjects()
{
  //draw frame
  gdi::drawBitmap(hdc_, 0, textHeightPx_, staticLayer_, false);
}

void Game::drawGui()
{
  HGDIOBJ replaced = SelectObject(textLayerDc_, textLayer_);
  //clean
  BitBlt(textLayerDc_, 0, 0, windowSize_ + borderSize_ * 2, textHeightPx_, textLayerDc_, 0, 0, WHITENESS);
  Rectangle(textLayerDc_, 1, 1, windowSize_ + borderSize_ * 2 - 1, textHeightPx_ - 1);
  UINT Time = GetTickCount() - startTime_;
  UINT Sec = (Time / MSEC_IN_SEC) % SEC_IN_MIN;
  UINT Min = ((Time / MSEC_IN_SEC / SEC_IN_MIN) % SEC_IN_MIN);
  std::string strOut = "Time " + std::to_string(Min) + ":" + std::to_string(Sec);
  TextOut(textLayerDc_, borderSize_, borderSize_, strOut.c_str(), strOut.length());
  strOut = "Score:" + std::to_string(score_) + " Lives: " + std::to_string(world_.getPlayer() ? world_.getPlayer()->getHP() : 0);
  SIZE textsize;
  GetTextExtentPoint32(textLayerDc_, strOut.c_str(), strOut.length(), &textsize);
  TextOut(textLayerDc_, windowSize_ + borderSize_ * 2 - textsize.cx - borderSize_, borderSize_, strOut.c_str(), strOut.length());

  BitBlt(hdc_, 0, 0, windowSize_ + borderSize_ * 2, textHeightPx_, textLayerDc_, 0, 0, SRCCOPY);
  //deselect a textLayer_ bitmap for use int other DC
  SelectObject(textLayerDc_, replaced);
}
