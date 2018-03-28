#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H
#include "stdafx.h"

struct PointComarator {
  bool operator() (const POINT& point1, const POINT& point2) const
  {
    if (point1.x != point2.x)
      return point1.x < point2.x;
    else
      return point1.y < point2.y;
  }
};
template <typename T> int sign(T value)
{
  return (value > 0) ? 1 : ((value < 0) ? -1 : 0);
};
/*
struct CompareGameObjectPoint
{
  CompareGameObjectPoint(const POINT& point) : point_(point) {}
  bool operator()(GameObject& object) const
  {
    return object.getPosition().x == point_.x && object.getPosition().y == point_.y;
  }
private:
  const POINT& point_;
};

class CompareGameObjectPoint
{
private:
  POINT point_;
public:
  CompareGameObjectPoint(const POINT& point)
    : point_(point)
  {}

  bool operator()(const GameObject * gameobject)
  {
    return gameobject->getPosition().x == point_.x && gameobject->getPosition().y == point_.y;
  }
};
*/


namespace gdi 
{
  extern HBRUSH createBrush(UINT style, COLORREF color, ULONG_PTR hatch);
  extern HPEN createPen(UINT style, COLORREF color, UINT width);
  extern void drawBitmap(HDC hdc, const int x, const int y, HBITMAP hBitmap, const bool transparent = false);
  extern void drawBitmap(HDC hdc, const int x, const int y, const int width, const int height, HBITMAP hBitmap, const bool transparent = false);
  extern void rotateTexture(HBITMAP hBitmap, const int degres, bool AdjustSize = false);
  extern void rotateTexture2(HBITMAP hBitmap, const int degres, bool AdjustSize = false);
  extern HBITMAP copyBitmap(HBITMAP hBitmap);
}
extern const double PI;

extern const int MAX_FPS;
extern const int MAP_SIZE;
extern const int TILE_SIZE;
extern int MAX_ENEMY_COUNT;

extern int WALL_HP;
extern int GOLD_HP;
extern int PLAYER_LIVES;

extern const int MSEC_IN_SEC;
extern const int SEC_IN_MIN;
extern const int MKS_IN_SEC;

extern int ENEMY_SHOOT_DELAY;
extern int PLAYER_SHOOT_DELAY;
extern int ENEMY_UPDATE_DELAY;

extern const double DEFAULT_BULLET_SPEED;
extern const double DEFAULT_OBJECT_SPEED;

extern int VISIBLE_DISTANCE;

extern bool DEBUG_DRAW_COLLISIONS;

extern const int KEY_UP;
extern const int KEY_DOWN;
extern const int KEY_LEFT;
extern const int KEY_RIGHT;

extern COLORREF WALL_COLOR;
extern COLORREF ALLY_COLOR;
extern COLORREF ENEMY_COLOR;
extern COLORREF COLOR_GREY;
extern COLORREF COLOR_YELLOW;
extern COLORREF COLOR_BLACK;
extern const std::string WALL_TEXTURE;
extern const std::string TANK_GREEN_1;
extern const std::string TANK_BLUE_1;
extern POINT DEFAULT_DIRECTION;

extern inline int SQR(int x);
extern double pointDistance(POINT first, POINT second);
extern bool circleIntersection(POINT first, POINT second, UINT radius1, UINT radius2);
extern INT64 getTimeMks(INT64 oldTimeMks = 0);

#endif