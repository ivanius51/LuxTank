#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>

#include <Windows.h>

class GameObject
{
public:
  GameObject(POINT position, UINT hp, int attackDamage);
  GameObject(int x, int y, UINT hp, int attackDamage);
  POINT getPosition();
  POINT getDirection();
  UINT getHP();
  int getAttackDamage();
  bool canMove();
  bool isWalkable();
  bool isDead();
  void takeDamage(int damage);
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
private:
  UINT hp_ = 0;
  int attackDamage_ = 0;
  bool canMove_ = false;
  bool isWalkable_ = true;
  POINT position_ = { 0,0 };
  POINT direction_ = { 0,-1 };
  POINT oldDirection_ = { 0,-1 };
  UINT32 updateTime_;
  UINT32 drawTime_;
};
class VisualObject :public GameObject
{
public:
  VisualObject(int x, int y, UINT hp, int attackDamage, COLORREF color = 0, COLORREF background = 0, UINT width = 0, UINT height = 0);
  COLORREF getColor();
  COLORREF getBackground();
  UINT getWidth();
  UINT getHeight();
protected:
  HDC targetdc_;
  HBITMAP targeBitmap_;
private:
  COLORREF color_;
  COLORREF background_;
  UINT width_;
  UINT height_;
};
class Wall :public VisualObject
{
public:
  Wall(int x, int y, UINT hp = 4, int attackDamage = 0, COLORREF color = RGB(0, 0, 128), COLORREF background = RGB(0, 0, 255), UINT width = 0, UINT height = 0);
  Wall(POINT point, UINT hp = 4, int attackDamage = 0, COLORREF color = RGB(0, 0, 128), COLORREF background = RGB(0, 0, 255), UINT width = 0, UINT height = 0);
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
  MovableObject(int x, int y, COLORREF color = RGB(0, 0, 0), COLORREF background = RGB(0, 0, 0), UINT hp = 1, int attackDamage = 1, UINT width = 0, UINT height = 0);
  void rotate(POINT point);
  void moveForward();
protected:
private:
};

class Tank;

class Bullet :public MovableObject
{
public:
  Bullet(Tank* tank);
  void draw();
  void update();
protected:
private:
  Tank* shooter_;
};
class Tank :public MovableObject
{
public:
  Tank(int x, int y, COLORREF color = RGB(0, 0, 0), COLORREF background = RGB(0, 0, 0), UINT hp = 1, int attackDamage = 1, UINT width = 0, UINT height = 0);
  Tank(POINT point, COLORREF color = RGB(0, 0, 0), COLORREF background = RGB(0, 0, 0), UINT hp = 1, int attackDamage = 1, UINT width = 0, UINT height = 0);
  void shoot();
  void draw();
  void drawTo(HDC hdc, HBITMAP hbitmap);
  void update();
protected:
private:
  std::vector<Bullet> bullets_;
};


#endif // GAMEOBJECT_H