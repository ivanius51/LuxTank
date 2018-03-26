//#pragma once
#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H
#include "GameObject.h"

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
};*/

extern inline int SQR(int x);

namespace gdi {
  extern void drawBitmap(HDC hdc, const int x, const int y, HBITMAP hBitmap, const bool transparent = false);
  extern void drawBitmap(HDC hdc, const int x, const int y, const int width, const int height, HBITMAP hBitmap, const bool transparent = false);
  extern void rotateTexture(HBITMAP hBitmap, const int degres, bool AdjustSize = false);
  extern void rotateTexture2(HBITMAP hBitmap, const int degres, bool AdjustSize = false);
  extern HBITMAP copyBitmap(HBITMAP hBitmap);
}
extern double PI;
extern int MAX_FPS;
extern int MAP_SIZE;
extern int TILE_SIZE;
extern int WALL_HP;
extern int GOLD_HP;
extern int PLAYER_LIVES;
extern int MSEC_IN_SEC;
extern int SEC_IN_MIN;
extern int SHOOT_DELAY;
extern int ENEMY_UPDATE_DELAY;
extern UINT DEFAULT_BULLET_SPEED;
extern int VISIBLE_DISTANCE;
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


#endif