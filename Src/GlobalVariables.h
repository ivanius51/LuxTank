//#pragma once
#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

struct POINTComarator {
  bool operator() (const POINT& point1, const POINT& point2) const
  {
    if (point1.x != point2.x)
      return point1.x < point2.x;
    else
      return point1.y < point2.y;
  }
};

bool isMooving(const GameObject* gameobject);
inline int SQR(int x);

extern int MAX_FPS;
extern int MAP_SIZE;
extern int TILE_SIZE;
extern int PLAYER_LIVES;
extern int MSEC_IN_SEC;
extern int SEC_IN_MIN;
extern int SHOOT_DELAY;
extern int VISIBLE_DISTANCE;
extern COLORREF WALL_COLOR;
extern COLORREF ALLY_COLOR;
extern COLORREF ENEMY_COLOR;
extern COLORREF COLOR_GREY;
extern COLORREF COLOR_BLACK;
extern POINT DEFAULT_DIRECTION;


#endif