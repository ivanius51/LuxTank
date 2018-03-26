#include <exception>

#include "stdafx.h"
#include <algorithm>
#include <string>

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
    mapSize_ = mapSize;
    tileSize_ = tileSize;
    windowSize_ = mapSize * tileSize;
    borderSize_ = 4;
    textHeightPx_ = 32 - borderSize_ * 2 + windowSize_ % 12;

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


    generateNewMap();

    GetWindowRect(handle, &Window);
    GetWindowRect(GetDesktopWindow(), &Screen);
    GetClientRect(handle, &Client);

    MoveWindow(handle, (Screen.right / 2) - (Window.right - Window.left) / 2, (Screen.bottom / 2) - (Window.bottom - Window.top) / 2, Window.right - Window.left, textHeightPx_ + windowSize_ + borderSize_ * 2, 1);
    SetWindowLong(handle, GWL_STYLE, WS_POPUP);
    SetWindowRgn(handle, CreateRectRgn(Client.left + borderSize_ / 2, Client.top + borderSize_ / 2, windowSize_ + borderSize_ * 2 + borderSize_ / 2, textHeightPx_ + windowSize_ + borderSize_ * 2), TRUE);
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

Command* Game::input()
{
  Command* command = nullptr;
  if (getPlayer())
  {
    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
      command = new RotateCommand(getPlayer(), 0, -1);
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
      command = new RotateCommand(getPlayer(), 0, 1);
    }
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
      command = new RotateCommand(getPlayer(), -1, 0);
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
      command = new RotateCommand(getPlayer(), 1, 0);
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
      command = new FireCommand(getPlayer());
    }
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    {
      stopGame();
    }
  }
  return command;
}

void Game::update()
{
  //std::map<POINT, GameObject*>::iterator
  //Objects update
  for (auto&& tile : tiles_)
    tile->update();

  for (auto&& bullet : bullets_)
    bullet->update();
  //remove collided(stopped) bullets
  bullets_.erase(
    std::remove_if(bullets_.begin(), bullets_.end(),
      [](std::shared_ptr<Bullet> bullet) {return !bullet->isMooving(); }
  ), bullets_.end());

  if (getPlayer() && getPlayer()->isDead() || getGold() && getGold()->isDead())
    stopGame();

  //remove Died(destroyed) objects
  tiles_.erase(
    std::remove_if(tiles_.begin(), tiles_.end(),
      [](std::shared_ptr<GameObject> object) {return object->isDead(); }
  ), tiles_.end());
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

bool Game::isRunning()
{
  return isRunning_;
}

void Game::increaseScore()
{
  score_++;
}

GameObject* Game::getPlayer()
{
  return player_.lock().get();
}
GameObject* Game::getGold()
{
  return gold_.lock().get();
}
GameObject* Game::getObject(POINT point)
{
  auto tile = std::find_if(tiles_.begin(), tiles_.end(), 
      [&point](std::shared_ptr<GameObject> object) 
      {return object->getPosition().x == point.x && object->getPosition().y == point.y; }//CompareGameObjectPoint(point)
  );
  if (tile != tiles_.end())
    return tile->get();
  else
    return nullptr;
}
bool Game::deleteObject(GameObject* gameobject)
{
  auto tile = std::find_if(tiles_.begin(), tiles_.end(),
    [&gameobject](std::shared_ptr<GameObject> object)
    {return object.get() == gameobject; }
  );
  if (tile != tiles_.end())
  {
    tiles_.erase(tile);
    return true;
  }
  return false;
  //for (auto it = tiles_.begin(); it != tiles_.end(); it++)
  //  if (it->get() == gameobject)
  //  {
  //    tiles_.erase(it);
  //    break;
  //  }
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
  return ((!gameobject) || gameobject->isWalkable());
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
  return (isValidPosition(x, y) && isWalkable(x, y));
}
bool Game::isInVisibleDistance(POINT first, POINT second)
{
  return (sqrt(SQR(second.x - first.x) + SQR(second.y - first.y)) < VISIBLE_DISTANCE);
}
bool Game::isIntersection(POINT first, POINT second)
{
  return ((first.x == second.x) || (first.y == second.y));
}
GameObject* Game::collidedWith(GameObject* gameobject)
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

  int distance = x * x + y * y;
  int maxdistance = (Radius2 + Radius1) * (Radius2 + Radius1);

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

void Game::addBullet(std::shared_ptr<Bullet> bullet)
{
  bullets_.push_back(bullet);
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

void Game::generateNewMap()
{
  POINT point;
  std::shared_ptr<GameObject> gameobject;
  //random walls
  for (int i = 0; i < mapSize_; i++)
    for (int j = 0; j < mapSize_ - 2; j++)
      if (rand() % 100 < 20)
      {
        point = { i, j };
        gameobject.reset(new Wall(point, WALL_HP, 0, WALL_TEXTURE));
        //gameobject.get()->bindUpdateCallback([](GameObject& gameobject) {if (gameobject.isDead()) Game::instance().stopGame(); });
        tiles_.push_back(std::shared_ptr<GameObject>(gameobject));
      }
  //Default player position
  point.x = mapSize_ / 2 + 2;
  point.y = mapSize_ - 1;
  gameobject.reset(new Tank(point, TANK_GREEN_1, ALLY_COLOR, PLAYER_LIVES));
  player_ = gameobject;
  dynamic_cast<Tank*>(player_.lock().get())->setPlayer(true);
  tiles_.push_back(std::shared_ptr<GameObject>(gameobject));

  for (int i = 0; i <= int(mapSize_ / 2 + 1); i++)
    for (int j = 0; j < mapSize_; j++)
    {
      point = { j, i };
      if ((i % 2 == 0) && (j % 2 == 0) && (rand() % 100 < 20) && (!getObject(point)))
      {
        gameobject.reset(new Tank(point, TANK_BLUE_1, ENEMY_COLOR, 1));
        dynamic_cast<Tank*>(gameobject.get())->setEnemy(true);
        tiles_.push_back(std::shared_ptr<GameObject>(gameobject));
      }
    }

  for (int i = int(mapSize_ / 2 - 1); i <= int(mapSize_ / 2 + 1); i++)
  {
    point = { i, int(mapSize_ - 2) };
    gameobject.reset(new Wall(point, WALL_HP, 0, WALL_TEXTURE));
    tiles_.push_back(std::shared_ptr<GameObject>(gameobject));
    point = { i, int(mapSize_ - 1) };
    if (i == (mapSize_ / 2))
    {
      gameobject.reset(new Wall(point, GOLD_HP, 0, COLOR_YELLOW / 2, COLOR_YELLOW));
      gold_ = gameobject;
      gameobject.get()->bindUpdateCallback([](GameObject& gameobject) {if (gameobject.isDead()) Game::instance().stopGame(); });
      tiles_.push_back(std::shared_ptr<GameObject>(gameobject));
    }
    else
    {
      gameobject.reset(new Wall(point, WALL_HP, 0, WALL_TEXTURE));
      tiles_.push_back(std::shared_ptr<GameObject>(gameobject));
    }
  }
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
  HGDIOBJ replacedBuffer = SelectObject(bufferDc_, buffer_);
  HGDIOBJ replacedStatic = SelectObject(staticLayerDc_, staticLayer_);

  //clear buufer
  BitBlt(bufferDc_, 0, 0, windowSize_, windowSize_, bufferDc_, 0, 0, BLACKNESS);
  //std::map<POINT, GameObject*>::const_iterator
  for (auto&& bullet : bullets_)
    bullet->draw();
  for (auto&& object : tiles_)
  {
    if (!object->canMove())
      object->draw();
  }
  for (auto&& object : tiles_)
  {
    if (object->canMove())
      object->draw();
  }

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
  strOut = "Score:" + std::to_string(score_) + " Lives: " + std::to_string(getPlayer()? getPlayer()->getHP(): 0);
  SIZE textsize;
  GetTextExtentPoint32(textLayerDc_, strOut.c_str(), strOut.length(), &textsize);
  TextOut(textLayerDc_, windowSize_ + borderSize_ * 2 - textsize.cx - borderSize_, borderSize_, strOut.c_str(), strOut.length());

  BitBlt(hdc_, 0, 0, windowSize_ + borderSize_ * 2, textHeightPx_, textLayerDc_, 0, 0, SRCCOPY);
  //deselect a textLayer_ bitmap for use int other DC
  SelectObject(textLayerDc_, replaced);
}
