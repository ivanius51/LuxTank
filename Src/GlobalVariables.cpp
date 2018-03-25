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
const std::string WALL_TEXTURE = "res/wall.bmp";
POINT DEFAULT_DIRECTION = { 0, -1 };

inline int SQR(int x)
{
  return x * x;
}
namespace gdi {
  void drawBitmap(HDC hdc, int x, int y, HBITMAP hBitmap, bool transparent)
  {
    if (hBitmap)
    {
      HDC hDC = CreateCompatibleDC(hdc);
      HGDIOBJ replaced = SelectObject(hDC, hBitmap);
      if (replaced)
      {
        //SetMapMode(hMemDC, GetMapMode(hdc));
        BITMAP bitmap;
        GetObject(hBitmap, sizeof(BITMAP), &bitmap);
        //POINT ptSize = { bitmap.bmWidth , bitmap.bmHeight };
        //DPtoLP(hdc, &ptSize, 1);
        if (transparent)
          BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hDC, 0, 0, SRCPAINT);
        else
          BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hDC, 0, 0, SRCCOPY);

        SelectObject(hDC, replaced);
      }
      DeleteDC(hDC);
    }
  }
  void drawBitmap(HDC hdc, int x, int y, int width, int height, HBITMAP hBitmap, bool transparent)
  {
    if (hBitmap)
    {
      HDC hDC = CreateCompatibleDC(hdc);
      HGDIOBJ replaced = SelectObject(hDC, hBitmap);
      if (replaced)
      {
        //SetMapMode(hMemDC, GetMapMode(hdc));
        BITMAP bitmap;
        GetObject(hBitmap, sizeof(BITMAP), &bitmap);
        //POINT ptSize = { bitmap.bmWidth , bitmap.bmHeight };
        //DPtoLP(hdc, &ptSize, 1);
        if (bitmap.bmWidth == width && bitmap.bmHeight == height)
        {
          if (transparent)
            BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hDC, 0, 0, SRCPAINT);
          else
            BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hDC, 0, 0, SRCCOPY);
        }
        else
        {
          if (transparent)
            StretchBlt(hdc, x, y, width, height, hDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCPAINT);
          else
            StretchBlt(hdc, x, y, width, height, hDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
        }
        SelectObject(hDC, replaced);
      }
      DeleteDC(hDC);
    }
  }
}

