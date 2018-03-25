//#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include <memory>
#define NOMINMAX
#include <Windows.h>

#include "GlobalVariables.h"

class GameObject
{
public:
  GameObject(POINT position, UINT hp, int attackDamage);
  GameObject(int x, int y, UINT hp, int attackDamage);
  virtual ~GameObject();
  POINT getPosition();
  POINT getDirection();
  UINT getHP();
  UINT getMaxHP();
  int getAttackDamage();
  bool canMove();
  bool isWalkable();
  bool isDead();
  void takeDamage(int damage);
  virtual void takeDamage(int damage, POINT damageDirection) = 0;
  virtual void draw() = 0;
  virtual void drawTo(HDC hdc, HBITMAP hbitmap) = 0;
  virtual void update() = 0;
protected:
  void setPosition(int x, int y);
  void setPosition(POINT point);
  void setDirection(int x, int y);
  void setDirection(POINT point);
  void setHP(UINT hp);
  void setAttackDamage(int damage);
  void setCanMove(bool move);
  void setIsWalkable(bool walkable);
  POINT getOldDirection();
private:
  UINT hp_ = 0;
  UINT maxHp_ = 0;
  int attackDamage_ = 0;
  bool canMove_ = false;
  bool isWalkable_ = true;
  POINT position_ = { 0, 0 };
  POINT direction_ = { 0, -1 };
  POINT oldDirection_ = { 0, -1 };
  UINT32 updateTime_;
  UINT32 drawTime_;
};
class VisualObject :public GameObject
{
public:
  VisualObject(int x, int y, UINT hp, int attackDamage, COLORREF color = 0, COLORREF background = 0, UINT width = 0, UINT height = 0);
  VisualObject(int x, int y, UINT hp, int attackDamage, COLORREF color = 0, COLORREF background = 0, const std::string& texture = "", UINT width = 0, UINT height = 0);
  ~VisualObject();
  COLORREF getColor();
  COLORREF getBackground();
  UINT getWidth();
  UINT getHeight();
  POINT getScreenPosition();
  POINT getOffset();
  HBITMAP getTexture();
  void takeDamage(int damage, POINT damageDirection);
protected:
  HDC targetdc_ = nullptr;
  HBITMAP targeBitmap_ = nullptr;
  RECT Destrucions = { 0, 0, 0, 0 };
  void setOffset(int x, int y);
  void setOffset(POINT point);
  void setWidth(UINT width);
  void setHeight(UINT height);
private:
  COLORREF color_ = 0;
  COLORREF background_ = 0;
  HBITMAP texture_ = nullptr;
  UINT width_ = 0;
  UINT height_ = 0;
  POINT offset_ = { 0,0 };
};
class Wall :public VisualObject
{
public:
  Wall(int x, int y, UINT hp = 5, int attackDamage = 0, COLORREF color = COLOR_GREY, COLORREF background = WALL_COLOR, UINT width = 0, UINT height = 0);
  Wall(POINT point, UINT hp = 5, int attackDamage = 0, COLORREF color = COLOR_GREY, COLORREF background = WALL_COLOR, UINT width = 0, UINT height = 0);
  Wall(int x, int y, UINT hp = 5, int attackDamage = 0, const std::string& texture = WALL_TEXTURE, UINT width = 0, UINT height = 0);
  Wall(POINT point, UINT hp = 5, int attackDamage = 0, const std::string& texture = WALL_TEXTURE, UINT width = 0, UINT height = 0);
  void draw();
  void drawTo(HDC hdc, HBITMAP hbitmap);
  void update();
protected:
private:
};
class Gold :public Wall
{
public:
  Gold(UINT x, UINT y);
  Gold(POINT point);
  void draw();
  void drawTo(HDC hdc, HBITMAP hbitmap);
  void update();
protected:
private:
};
class MovableObject :public VisualObject
{
public:
  MovableObject(int x, int y, COLORREF color = COLOR_BLACK, COLORREF background = COLOR_BLACK, UINT hp = 1, int attackDamage = 1, UINT width = 0, UINT height = 0);
  void rotate(POINT point);
  void rotateTo(POINT point);
  void moveForward();
  void stop();
  bool isMooving();
  bool canUpdate();
  UINT getUpdateTime();
  UINT getUpdateDelay();
  void setUpdateDelay(UINT updateDelay);
protected:
  UINT updateTime_ = 0;
private:
  bool isMooving_ = false;
  UINT updateDelay_ = 0;
};

class Tank;

class Bullet :public MovableObject
{
public:
  Bullet(Tank* tank);
  void draw();
  void drawTo(HDC hdc, HBITMAP hbitmap);
  void update();
protected:
private:
  //std::weak_ptr<Tank> shooter_;
  //Tank * shooter_ = nullptr;
};
class Tank :public MovableObject
{
public:
  Tank(int x, int y, COLORREF color = COLOR_BLACK, COLORREF background = COLOR_BLACK, UINT hp = 1, int attackDamage = 1, UINT width = 0, UINT height = 0);
  Tank(POINT point, COLORREF color = COLOR_BLACK, COLORREF background = COLOR_BLACK, UINT hp = 1, int attackDamage = 1, UINT width = 0, UINT height = 0);
  void shoot();
  void draw();
  void drawTo(HDC hdc, HBITMAP hbitmap);
  void update();
  bool isEnemy();
  bool isPlayer();
protected:
private:
  //Bullet * bullet_ = nullptr;
  UINT ShootTime_;
  bool isenemy_ = false;
  bool isplayer_ = false;
};


#endif // GAMEOBJECT_H