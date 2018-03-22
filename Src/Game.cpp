#include <exception>

#include "stdafx.h"
#include <algorithm>
#include <string>
#include <map>

#include "GameObject.h"
#include "Game.h"

Game & Game::instance()
{
  static Game* instance = new Game();
  return *instance;
}

void Game::initialization(HWND handle, UINT mapSize, UINT tileSize, UINT fpsmax)
{
  if (((tileSize & (tileSize - 1)) != 0) || tileSize < 16 || tileSize > 256)
    throw std::invalid_argument("tile size must be a power of 2 and hightest that 8 and lower than 256");

  if (handle != INVALID_HANDLE_VALUE)
  {
    handle_ = handle;
    hdc_ = GetDC(handle_);
    mapSize_ = mapSize;
    tileSize_ = tileSize;
    windowSize_ = mapSize * tileSize;
    textHeightPx_ = 24 + windowSize_ % 12;
    if (fpsmax > 1000)
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
    int y = int(windowSize_ / 12) + 2;//12px and two line for text
    //resize console
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(consoleOutputHandle, &bufferInfo);
    SMALL_RECT& windowInfo = bufferInfo.srWindow;
    COORD windowSize = { windowInfo.Right - windowInfo.Left + 1, windowInfo.Bottom - windowInfo.Top + 1 };
    if (windowSize.X > x || windowSize.Y > y)
    {
      SMALL_RECT windowinfo = { 0, 0, x < windowSize.X ? x - 1 : windowSize.X - 1, y < windowSize.Y ? y - 1 : windowSize.Y - 1 };
      SetConsoleWindowInfo(consoleOutputHandle, TRUE, &windowinfo);
    }
    COORD size = { x, y };
    SetConsoleScreenBufferSize(consoleOutputHandle, size);
    SMALL_RECT windowinfo = { 0, 0, x - 1, y - 1 };
    SetConsoleWindowInfo(consoleOutputHandle, TRUE, &windowinfo);
    //prepare window
    system("cls");
    SetBkMode(hdc_, TRANSPARENT);

    //Create Text Layer DC,Bitmap
    textLayerDc_ = CreateCompatibleDC(hdc_);
    if (!textLayerDc_)
      throw std::runtime_error("can't Create DC textLayer");
    textLayer_ = CreateCompatibleBitmap(hdc_, windowSize_, textHeightPx_);
    if (!textLayer_)
      throw std::runtime_error("can't Create Bitmap textLayer");
    HFONT hFont = CreateFont(20, 6, 0, 0, FW_DONTCARE, 0, 0, 0, ANSI_CHARSET,
      OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, VARIABLE_PITCH,
      "SYSTEM_FIXED_FONT");
    BitBlt(hdc_, 0, textHeightPx_, windowSize_, windowSize_, hdc_, 0, 0, BLACKNESS);

    bufferDc_ = CreateCompatibleDC(hdc_);
    if (!bufferDc_)
      throw std::runtime_error("can't Create DC Buffer");
    buffer_ = CreateCompatibleBitmap(hdc_, windowSize_, windowSize_);
    if (!buffer_)
      throw std::runtime_error("can't Create Bitmap Buffer");

    generateNewMap();

    startTime_ = GetTickCount();
  }
  else
    throw std::runtime_error("initialization handle is invalid");
}

void Game::free()
{
  ReleaseDC(handle_, hdc_);
  DeleteDC(textLayerDc_);
  DeleteObject(textLayer_);
}

Command* Game::input()
{
  if (GetAsyncKeyState(VK_UP))
  {
    return nullptr;
  }
  if (GetAsyncKeyState(VK_DOWN))
  {
    return nullptr;
  }
  if (GetAsyncKeyState(VK_LEFT))
  {
    return nullptr;
  }
  if (GetAsyncKeyState(VK_RIGHT))
  {
    return nullptr;
  }
  if (GetAsyncKeyState(VK_SPACE))
  {
    return nullptr;
  }
  if (GetAsyncKeyState(VK_ESCAPE))
  {
    return nullptr;
  }
  return nullptr;
}

void Game::update()
{
}

void Game::draw()
{
  drawObjects();
  renderObjects();
  drawGui();
}

Tank* Game::getPlayer()
{
  return player_;
}

Gold* Game::getGold()
{
  return gold_;
}

void Game::increaseScore()
{
  score_++;
}

GameObject* Game::getObject(POINT point)
{
  auto tile = tiles_.find(point);
  if (tile != tiles_.end())
    return tile->second;
  else
    return nullptr;
}

GameObject* Game::getObject(int x, int y)
{
  POINT point;
  point.x = x;
  point.y = y;
  return Game::getObject(point);
}

bool Game::isWalkable(int x, int y)
{
  GameObject* gameobject = Game::getObject(x, y);
  return ((gameobject) && gameobject->isWalkable());
}
bool Game::isWalkable(POINT point)
{
  return isWalkable(point.x, point.y);
}

bool Game::isValidPosition(POINT point)
{
  return isValidPosition(point.x, point.y);
}
bool Game::isValidPosition(int x, int y)
{
  return ((x >= 0) && (x < mapSize_)) && ((y >= 0) && (y < mapSize_));
}

bool Game::canMoveTo(POINT point)
{
  return canMoveTo(point.x, point.y);
}
bool Game::canMoveTo(int x, int y)
{
  return isValidPosition(x, y) && isWalkable(x, y);
}

GameObject* Game::checkCollision(GameObject* gameobject)
{
  if (!dynamic_cast<VisualObject*>(gameobject))
    return nullptr;

  POINT Direction = gameobject->getDirection();
  POINT Position = gameobject->getPosition();
  POINT Offset = dynamic_cast<VisualObject*>(gameobject)->getOffset();
  Position.x += (Offset.x + (Direction.x * tileSize_ / 2)) / 2;
  Position.y += (Offset.y + (Direction.y * tileSize_ / 2)) / 2;
  GameObject* interact = getObject(Position);
  if (!interact || !dynamic_cast<VisualObject*>(interact))
    return nullptr;

  POINT ScreenPosition1 = dynamic_cast<VisualObject*>(gameobject)->getScreenPosition();
  int Radius1 = std::max(dynamic_cast<VisualObject*>(gameobject)->getWidth(), dynamic_cast<VisualObject*>(gameobject)->getHeight()) / 2;
  POINT ScreenPosition2 = dynamic_cast<VisualObject*>(interact)->getScreenPosition();
  int Radius2 = std::max(dynamic_cast<VisualObject*>(interact)->getWidth(), dynamic_cast<VisualObject*>(interact)->getHeight()) / 2;

  int x = ScreenPosition1.x - ScreenPosition2.x;
  int y = ScreenPosition1.y - ScreenPosition2.y;

  float distance = x * x + y * y;
  float maxdistance = (Radius2 + Radius1) * (Radius2 + Radius1);

  if (distance <= maxdistance)
    return interact;
  else
    return nullptr;
}


UINT Game::getFrameDelay()
{
  return frameDelay_;
}

UINT Game::getWindowSize()
{
  return windowSize_;
}

UINT Game::getTileSize()
{
  return tileSize_;
}

UINT Game::getMapSize()
{
  return mapSize_;
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

void Game::drawBitmap(HDC hdc, int x, int y, HBITMAP hBitmap, bool transparent)
{
  if (hBitmap)
  {
    HDC hMemDC = CreateCompatibleDC(hdc);
    HGDIOBJ replaced = SelectObject(hMemDC, hBitmap);
    if (replaced)
    {
      //SetMapMode(hMemDC, GetMapMode(hdc));
      BITMAP bitmap;
      GetObject(hBitmap, sizeof(BITMAP), &bitmap);
      //POINT ptSize = { bitmap.bmWidth , bitmap.bmHeight };
      //DPtoLP(hdc, &ptSize, 1);
      if (transparent)
        BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hMemDC, 0, 0, SRCPAINT);
      else
        BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hMemDC, 0, 0, SRCCOPY);

      SelectObject(hMemDC, replaced);
    }
    DeleteDC(hMemDC);
  }
}

void Game::drawBitmap(int x, int y, HBITMAP hBitmap, bool transparent)
{
  Game::drawBitmap(hdc_, x, y, hBitmap, transparent);
}

Game::Game()
{
}

Game::~Game()
{
}

void Game::generateNewMap()
{
  POINT point;
  std::pair<std::map<POINT, GameObject*>::iterator, bool > result;
  for (int i = 0; i < mapSize_; i++)
    for (int j = 0; j < mapSize_ - 2; j++)
    {
      point.x = i;
      point.y = j;
      result = tiles_.insert(std::pair<POINT, GameObject*>({ point }, new Wall(point)));
    }

  point.x = mapSize_ / 2;
  point.y = mapSize_ - 1;

  result = tiles_.insert(std::pair<POINT, GameObject*>({ point }, new Gold(point)));

  point.x = mapSize_ / 2 + 2;
  point.y = mapSize_ - 1;
  player_ = new Tank(point, RGB(0, 128, 0), RGB(0, 256, 0), 3);
  result = tiles_.insert(std::pair<POINT, GameObject*>({ point }, player_));
}

void Game::renderObjects()
{
  for (std::map<POINT, GameObject*>::iterator i = tiles_.begin(); i != tiles_.end(); i++)
  {
    i->second->draw();
  }

  drawBitmap(hdc_, 0, textHeightPx_, buffer_, false);
}

void Game::drawObjects()
{
  HGDIOBJ replaced = SelectObject(bufferDc_, buffer_);

  BitBlt(bufferDc_, 0, 0, windowSize_, windowSize_, bufferDc_, 0, 0, WHITENESS);
  Rectangle(bufferDc_, 1, 1, windowSize_ - 1, windowSize_ - 1);

  SelectObject(bufferDc_, replaced);

  //drawBitmap(hdc_, 0, textHeightPx_, buffer_, false);
}

void Game::drawGui()
{
  HGDIOBJ replaced = SelectObject(textLayerDc_, textLayer_);
  //clean
  BitBlt(textLayerDc_, 0, 0, windowSize_, textHeightPx_, textLayerDc_, 0, 0, WHITENESS);
  Rectangle(textLayerDc_, 1, 1, windowSize_ - 1, textHeightPx_ - 1);
  UINT Time = GetTickCount() - startTime_;
  UINT Sec = (Time / 1000) % 60;
  UINT Min = ((Time / 60000) % 60);
  std::string strOut = "Time " + std::to_string(Min) + ":" + std::to_string(Sec);
  TextOut(textLayerDc_, 2, 2, strOut.c_str(), strOut.length());
  strOut = "Score:" + std::to_string(score_) + " Lives: " + std::to_string(player_->getHP());
  SIZE textsize;
  GetTextExtentPoint32(textLayerDc_, strOut.c_str(), strOut.length(), &textsize);
  TextOut(textLayerDc_, windowSize_ - textsize.cx - 2, 2, strOut.c_str(), strOut.length());
  //deselect a textLayer_ bitmap for use int other DC
  SelectObject(textLayerDc_, replaced);
  drawBitmap(hdc_, 0, 0, textLayer_, false);
}
