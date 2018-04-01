#include "stdafx.h"
#include "Game.h"

#include <exception>
#include <iostream>
#include <conio.h>
#include <math.h>

#include "GameObject.h"
#include "GraphicEngine.h"


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

    frameDelay_ = lround(MKS_IN_SEC / fpsmax);

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
    textLayer_ = CreateCompatibleBitmap(hdc_, windowSize_ + borderSize_ * 2, textHeightPx_ + windowSize_ + borderSize_ * 2);
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
    if (!staticLayerDc_)
      throw std::runtime_error("can't Create DC Buffer");
    staticLayer_ = CreateCompatibleBitmap(hdc_, windowSize_ + borderSize_ * 2, windowSize_ + borderSize_ * 2);
    if (!staticLayer_)
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
      int symbol = _getch();
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
      //command->execute();
      //delete command;
      //command = nullptr;
      inputs_.push_back(std::unique_ptr<Command>(command));
      command = nullptr;
    }

    //multiple commands input modifier
    if (GetAsyncKeyState(VK_SPACE) & 0x8001)
      command = new FireCommand(world_.getPlayer());
    if (command)
    {
      //command->execute();
      //delete command;
      //command = nullptr;
      inputs_.push_back(std::unique_ptr<Command>(command));
      command = nullptr;
    }
  }
  //system commands execute directly
  if (GetAsyncKeyState(VK_ESCAPE) & 0x8001)
    stopGame();
  if (GetAsyncKeyState(VK_PAUSE) & 0x8001)
  {
    if (isPaused())
      resume();
    else
      pause();
    Sleep(250);
  }
}

void Game::useInput()
{
  for (const auto& input : inputs_)
    input->execute();
  clearInput();
}

void Game::clearInput()
{
  for (auto&& input : inputs_)
    input.reset();
  inputs_.clear();
}

void Game::addBullet(std::shared_ptr<Bullet> bullet)
{
  world_.getBullets()->push_back(bullet);
}

void Game::addBullet(Bullet* bullet)
{
  world_.getBullets()->push_back(std::shared_ptr<Bullet>(bullet));
}

void Game::update(double elapsed)
{
  auto tiles = world_.getTiles();
  auto bullets = world_.getBullets();

  //Objects update
  for (auto&& tile : *tiles)
    tile->update(elapsed);

  for (auto&& bullet : *bullets)
    bullet->update(elapsed);

  gameOver_ = world_.isNoEnemy() || world_.getPlayer() && world_.getPlayer()->isDead() || world_.getGold() && world_.getGold()->isDead();
  if (gameOver_)
  {
    //pause();
    if (!world_.isNoEnemy())
    {
      //GameOver
      gameResult_ = "GameOver";
    }
    else
    {
      //Victory
      gameResult_ = "Victory";
    }
    showResult();
    //pause();
  }

  /*
  //remove collided(stopped) bullets
  bullets_.erase(
    std::remove_if(bullets_.begin(), bullets_.end(),
      [](std::shared_ptr<Bullet> bullet) {return !bullet->isMooving(); }
  ), bullets_.end());

  //remove Died(destroyed) objects
  tiles->erase(
    std::remove_if(tiles->begin(), tiles->end(),
      [](std::shared_ptr<GameObject> object) {return object->isDead(); }
  ), tiles->end());
  */
  world_.bulletsClear();
  world_.objectsClear();
}

void Game::renderFrame()
{
  INT64 startDrawTime = getTimeMks();// GetTickCount();
  drawBorder();
  renderObjects();
  drawObjects();
  drawGui();

  frameCounter++;
  frameTime = getTimeMks(startDrawTime);//GetTickCount() - startDrawTime;
  if (avgFrameTime == 0)
    avgFrameTime = frameTime;
  else
    avgFrameTime = (frameTime + avgFrameTime) / 2;
}

void Game::draw()
{
  drawBitmap(0, 0, textLayer_, false);
}

void Game::test()
{

}

void Game::startGame()
{
  //need draw this to other bitmap, but not have time right now
  renderFrame();
  gdi::drawBitmap(hdc_,0,0,buffer_);
  //draw();
  UINT tilesize = world_.getTileSize();
  HFONT hfont = NULL;
  HGDIOBJ oldfont = SelectObject(staticLayerDc_, hfont);
  HGDIOBJ replacedStatic;
  SIZE textsize;
  SetTextColor(staticLayerDc_, RGB(255, 255, 255));
  SetBkMode(staticLayerDc_, TRANSPARENT);
  std::string strOut;
  /*
  for (int i = 159; i > 0; i--)
  {
    UINT startTime = GetTickCount();
    drawBorder();
    renderObjects();
    replacedStatic = SelectObject(staticLayerDc_, staticLayer_);
    hfont = CreateFont(tilesize * ((i % 40) / -2), 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "SYSTEM_FIXED_FONT");
    SelectObject(staticLayerDc_, hfont);
    if (i > 40)
      strOut = std::to_string(i / 40);
    else
      strOut = "GO";
    GetTextExtentPoint32(staticLayerDc_, strOut.c_str(), strOut.length(), &textsize);
    TextOut(staticLayerDc_, windowSize_ / 2 - textsize.cx / 2, windowSize_ / 2 - textsize.cy / 2, strOut.c_str(), strOut.length());
    SelectObject(staticLayerDc_, replacedStatic);
    drawObjects();
    draw();
    int elapsed = (GetTickCount() - startTime);
    if (elapsed < 25)
      Sleep(25 - elapsed);
    DeleteObject(replacedStatic);
    DeleteObject(hfont);
  }
  */
  SelectObject(staticLayerDc_, oldfont);
  DeleteObject(hfont);
  resume();
  startTime_ = GetTickCount();
  //some test
  pause();
  BitBlt(hdc_, 0, 0, 1000, 1000, hdc_, 0, 0, BLACKNESS);
  
  gdi::Bitmap bitmap(windowSize_, windowSize_);
  bitmap.canvas.draw(buffer_);
  BitBlt(hdc_, 0, 0, windowSize_, windowSize_, bitmap.canvas.getDC(), 0, 0, SRCCOPY);
  bitmap.saveToFile("D:\\test24.bmp");
  //bitmap.setBitsPerPixel(16);
  //bitmap.saveToFile("D:\\test16.bmp");
  //bitmap.setBitsPerPixel(8);
  //bitmap.saveToFile("D:\\test8.bmp");
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

bool Game::isGameOver()
{
  return gameOver_;
}

void Game::increaseScore()
{
  score_++;
}

void Game::showResult()
{
  UINT tilesize = world_.getTileSize();
  SIZE textsize;
  SetTextColor(staticLayerDc_, RGB(255, 255, 255));
  SetBkMode(staticLayerDc_, TRANSPARENT);
  drawBorder();
  renderObjects();
  HGDIOBJ replacedStatic = SelectObject(staticLayerDc_, staticLayer_);
  HFONT hfont = CreateFont(tilesize * -3, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "SYSTEM_FIXED_FONT");
  HGDIOBJ oldfont = SelectObject(staticLayerDc_, hfont);
  SelectObject(staticLayerDc_, hfont);
  GetTextExtentPoint32(staticLayerDc_, gameResult_.c_str(), gameResult_.length(), &textsize);
  TextOut(staticLayerDc_, windowSize_ / 2 - textsize.cx / 2, windowSize_ / 2 - textsize.cy / 2, gameResult_.c_str(), gameResult_.length());
  SelectObject(staticLayerDc_, replacedStatic);
  drawObjects();
  DeleteObject(replacedStatic);
  DeleteObject(hfont);
  SelectObject(staticLayerDc_, oldfont);
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
  auto bullets = *world_.getBullets();
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
  //draw objects frame
  HGDIOBJ replaced = SelectObject(textLayerDc_, textLayer_);
  gdi::drawBitmap(textLayerDc_, 0, textHeightPx_, staticLayer_, false);
  SelectObject(textLayerDc_, replaced);
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

  //BitBlt(hdc_, 0, 0, windowSize_ + borderSize_ * 2, textHeightPx_, textLayerDc_, 0, 0, SRCCOPY);
  //deselect a textLayer_ bitmap for use int other DC
  SelectObject(textLayerDc_, replaced);
}
