#include "stdafx.h"
#include <Windows.h>
#include "GameObject.h"

int MAX_FPS = 60;
int MAP_SIZE = 15;
int TILE_SIZE = 32;
int PLAYER_LIVES = 3;
int MSEC_IN_SEC = 1000;
int SEC_IN_MIN = 60;
int SHOOT_DELAY = 250;
int VISIBLE_DISTANCE = 6;
COLORREF WALL_COLOR = RGB(156, 74, 0);
COLORREF ALLY_COLOR = RGB(0, 128, 0);
COLORREF ENEMY_COLOR = RGB(128, 0, 0);
COLORREF COLOR_GREY = RGB(128, 128, 128);
COLORREF COLOR_BLACK = 0;
POINT DEFAULT_DIRECTION = { 0, -1 };

bool isMooving(const GameObject* gameobject)
{
  if (dynamic_cast<MovableObject*>(gameobject))
    return dynamic_cast<MovableObject*>(gameobject)->isMooving();
  else
    return false;
}

inline int SQR(int x)
{
  return x*x;
}

template <typename T> int sign(T value) {
    return (value > 0) ? 1 : ((value < 0) ? -1 : 0);
}