#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include <functional>

#include "GraphicEngine.h"
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
  virtual void update(double elapsed) = 0;
  void bindUpdateCallback(std::function<void(GameObject&)> function);
  void updateCallback(GameObject& gameobject);
  void updateCallback();
  bool isSamePosition(const POINT & point);
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
  void setOldDirection();
  double frameTime_ = 0;
private:
  int hp_ = 0;
  UINT maxHp_ = 0;
  int attackDamage_ = 0;
  bool canMove_ = false;
  bool isWalkable_ = true;
  POINT position_ = { 0, 0 };
  POINT direction_ = { 0, -1 };
  POINT oldDirection_ = { 0, -1 };
  std::vector<std::function<void(GameObject&)>> updateCallbacks_;
  UINT32 drawTime_;
};
class VisualObject :public GameObject
{
public:
  VisualObject(int x, int y, UINT hp, int attackDamage, COLORREF color = 0, COLORREF background = 0, UINT width = 0, UINT height = 0);
  VisualObject(int x, int y, UINT hp, int attackDamage, COLORREF color = 0, COLORREF background = 0, const std::string& texture = "", UINT width = 0, UINT height = 0);
  ~VisualObject();
  virtual void draw();
  COLORREF getColor();
  COLORREF getBackground();
  UINT getWidth();
  UINT getHeight();
  POINT getScreenPosition();
  POINT getScreenPositionCenter();
  POINT getOffset();
  HBITMAP getTexture();
  void takeDamage(int damage, POINT damageDirection);
protected:
  virtual bool beginDraw(HDC hdc, HBITMAP hbitmap, bool trasparent = true);
  virtual void endDraw();
  void drawGizmo();
  void changeBrush(UINT style = BS_SOLID,COLORREF color = 0, ULONG_PTR hatch = 0);
  void changePen(UINT style = PS_SOLID, COLORREF color = 0, UINT width = 1);
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
  POINT offset_ = { 0, 0 };
  HPEN pen_ = nullptr;
  HBRUSH brush_ = nullptr;
  HBRUSH gizmoBrush_ = nullptr;
  //temporary
  HGDIOBJ tempPen_ = nullptr;
  HGDIOBJ tempBrush_ = nullptr;
  HDC targetdc_ = nullptr;
  HBITMAP targeBitmap_ = nullptr;
};
class Wall :public VisualObject
{
public:
  Wall(int x, int y, UINT hp = WALL_HP, int attackDamage = 0, COLORREF color = COLOR_GREY, COLORREF background = WALL_COLOR, UINT width = 0, UINT height = 0);
  Wall(POINT point, UINT hp = WALL_HP, int attackDamage = 0, COLORREF color = COLOR_GREY, COLORREF background = WALL_COLOR, UINT width = 0, UINT height = 0);
  Wall(int x, int y, UINT hp = WALL_HP, int attackDamage = 0, const std::string& texture = WALL_TEXTURE, COLORREF color = COLOR_BLACK, UINT width = 0, UINT height = 0);
  Wall(POINT point, UINT hp = WALL_HP, int attackDamage = 0, const std::string& texture = WALL_TEXTURE, COLORREF color = COLOR_BLACK, UINT width = 0, UINT height = 0);
  void draw();
  void drawTo(HDC hdc, HBITMAP hbitmap);
  void update(double elapsed);
protected:
private:
};
class MovableObject :public VisualObject
{
public:
  MovableObject(int x, int y, COLORREF color = COLOR_BLACK, COLORREF background = COLOR_BLACK, UINT hp = 1, int attackDamage = 1, UINT width = 0, UINT height = 0);
  MovableObject(int x, int y, UINT hp = 5, int attackDamage = 0, const std::string& texture = "", COLORREF color = COLOR_BLACK, COLORREF background = COLOR_BLACK, UINT width = 0, UINT height = 0);
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
  UINT updateDelay_ = ENEMY_UPDATE_DELAY;
  POINT nextPosition_ = { 0, 0 };
};

class Tank;

class Bullet :public MovableObject
{
public:
  Bullet(Tank* tank, UINT speed = DEFAULT_BULLET_SPEED);
  void draw();
  void drawTo(HDC hdc, HBITMAP hbitmap);
  void update();
  void update(double elapsed);
  bool hitTest(POINT position);
protected:
private:
  UINT speed_;
  //std::weak_ptr<Tank> shooter_;
  Tank* shooter_ = nullptr;
};
class Tank :public MovableObject
{
public:
  Tank(int x, int y, COLORREF color = COLOR_BLACK, COLORREF background = COLOR_BLACK, UINT hp = 1, int attackDamage = 1, UINT width = 0, UINT height = 0);
  Tank(POINT point, COLORREF color = COLOR_BLACK, COLORREF background = COLOR_BLACK, UINT hp = 1, int attackDamage = 1, UINT width = 0, UINT height = 0);
  Tank(int x, int y, const std::string& texture = "", COLORREF color = COLOR_BLACK, UINT hp = 1, int attackDamage = 1, UINT width = 0, UINT height = 0);
  Tank(POINT point, const std::string& texture = "", COLORREF color = COLOR_BLACK, UINT hp = 1, int attackDamage = 1, UINT width = 0, UINT height = 0);
  void shoot();
  void draw();
  void drawTo(HDC hdc, HBITMAP hbitmap);
  void update(double elapsed);
  bool isEnemy();
  bool isEnemy(GameObject* gamobject);
  void setEnemy(bool enemy);
  bool isPlayer();
  void setPlayer(bool player);
  void setShootDelay(UINT shootDelay);
protected:
private:
  UINT shootTime_;
  UINT shootDelay_ = ENEMY_SHOOT_DELAY;
  bool isenemy_ = false;
  bool isplayer_ = false;
};


#endif // GAMEOBJECT_H