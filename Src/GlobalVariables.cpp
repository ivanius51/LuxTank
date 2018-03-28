#include "stdafx.h"
#include <Windows.h>
#include "GameObject.h"

double PI = 3.141592;

int MAX_FPS = 60;

int MAP_SIZE = 15;
int TILE_SIZE = 32;
int MAX_ENEMY_COUNT = 1;

int PLAYER_LIVES = 3;
int WALL_HP = 4;
int GOLD_HP = 1;

int MSEC_IN_SEC = 1000;
int SEC_IN_MIN = 60;

int ENEMY_UPDATE_DELAY = 750;
int ENEMY_SHOOT_DELAY = 750;
int PLAYER_SHOOT_DELAY = 250;

double DEFAULT_BULLET_SPEED = 0.14;
UINT DEFAULT_OBJECT_SPEED = 1;
int VISIBLE_DISTANCE = 6;

bool DEBUG_DRAW_COLLISIONS = false;

int KEY_UP = 72;
int KEY_DOWN = 80;
int KEY_LEFT = 75;
int KEY_RIGHT = 77;

COLORREF WALL_COLOR = RGB(156, 74, 0);
COLORREF ALLY_COLOR = RGB(0, 128, 0);
COLORREF ENEMY_COLOR = RGB(128, 0, 0);
COLORREF COLOR_GREY = RGB(128, 128, 128);
COLORREF COLOR_YELLOW = RGB(255, 255, 0);
COLORREF COLOR_BLACK = 0;

const std::string WALL_TEXTURE = "res/wall.bmp";
const std::string TANK_GREEN_1 = "res/tankgreen.bmp";
const std::string TANK_BLUE_1 = "res/tankblue.bmp";
POINT DEFAULT_DIRECTION = { 0, -1 };

bool CompareGameObjectPoint(GameObject & gameobject, POINT & point)
{
  return point.y == gameobject.getPosition().y && point.x == gameobject.getPosition().x;
}

inline int SQR(int x)
{
  return x * x;
}
double pointDistance(POINT first, POINT second)
{
  return sqrt(SQR(second.x - first.x) + SQR(second.y - first.y));
}
bool circleIntersection(POINT first, POINT second, UINT radius1, UINT radius2)
{
  int distance = int(pointDistance(first, second));
  return distance <= (radius1 + radius2);//std::abs(int(radius1 - radius2)) <= 
}
namespace gdi {
  HBRUSH createBrush(UINT style, COLORREF color, ULONG_PTR hatch)
  {
    //Brush paint style
    LOGBRUSH logbrush;
    //BS_SOLID, BS_NULL, BS_HOLLOW, BS_HATCHED, BS_PATTERN, BS_PATTERN8X8, BS_DIBPATTERN, BS_DIBPATTERN8X8, BS_DIBPATTERNPT 
    logbrush.lbStyle = style;
    logbrush.lbColor = color;
    //HS_HORIZONTAL, HS_VERTICAL, HS_FDIAGONAL, HS_BDIAGONAL, HS_CROSS, HS_DIAGCROSS
    logbrush.lbHatch = hatch;
    return CreateBrushIndirect(&logbrush);
  }

  HPEN createPen(UINT style, COLORREF color, UINT width)
  {
    //psSolid, psDash, psDot, psDashDot, psDashDotDot, psClear, psInsideFrame, psUserStyle, psAlternate 
    return CreatePen(style, width, color);
  }
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
  void rotateTexture(HBITMAP hBitmap, const int degres, bool AdjustSize)
  {
    if (degres % 360 == 0)
      return;
    HDC hDC = CreateCompatibleDC(NULL);
    HGDIOBJ replaced = SelectObject(hDC, hBitmap);
    if (replaced)
    {
      BITMAP bitmap;
      GetObject(hBitmap, sizeof(BITMAP), &bitmap);
      {
        POINT Points[3];
        double radians = (degres * PI) / 180;
        double Cosinus = cos(radians);
        double Sinus = sin(radians);
        int Width = bitmap.bmWidth;
        int Height = bitmap.bmHeight;
        if (AdjustSize)
        {
          Width = round(bitmap.bmWidth * abs(Cosinus) + bitmap.bmHeight * abs(Sinus));
          Height = round(bitmap.bmWidth * abs(Sinus) + bitmap.bmHeight * abs(Cosinus));
        }
        int OffsetX = (Width - Width * Cosinus + Height * Sinus) / 2;
        int OffsetY = (Height - Width * Sinus - Height * Cosinus) / 2;
        Points[0].x = round(OffsetX);
        Points[0].y = round(OffsetY);
        Points[1].x = round(OffsetX + Width * Cosinus);
        Points[1].y = round(OffsetY + Width * Sinus);
        Points[2].x = round(OffsetX - Height * Sinus);
        Points[2].y = round(OffsetY + Height * Cosinus);
        PlgBlt(hDC, Points, hDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, 0, 0, 0);
      }
      SelectObject(hDC, replaced);
    }
    DeleteDC(hDC);
  }
  void rotateTexture2(HBITMAP hBitmap, const int degres, bool AdjustSize)
  {
    if (degres % 360 == 0)
      return;
    HDC hDC = CreateCompatibleDC(NULL);
    HGDIOBJ replaced = SelectObject(hDC, hBitmap);
    if (replaced)
    {
      BITMAP bitmap;
      GetObject(hBitmap, sizeof(BITMAP), &bitmap);
      {
        POINT Points[3];
        double radians = (degres * PI) / 180;
        double Cosinus = cos(radians);
        double Sinus = sin(radians);
        XFORM xform;
        xform.eM11 = Cosinus;
        xform.eM12 = Sinus;
        xform.eM21 = -Sinus;
        xform.eM22 = Cosinus;
        int Width;
        int Height;
        if (AdjustSize)
        {
          Width = round(bitmap.bmWidth * abs(Cosinus) + bitmap.bmHeight * abs(Sinus));
          Height = round(bitmap.bmWidth * abs(Sinus) + bitmap.bmHeight * abs(Cosinus));
        }
        else
        {
          Width = bitmap.bmWidth;
          Height = bitmap.bmHeight;
        }
        xform.eDx = (Width - Width * Cosinus + Height * Sinus) / 2;
        xform.eDy = (Height - Width * Sinus - Height * Cosinus) / 2;
        SetGraphicsMode(hDC, GM_ADVANCED);
        SetWorldTransform(hDC, &xform);
        BitBlt(hDC, 0, 0, Width, Height, hDC, 0, 0, SRCCOPY);
      }
      SelectObject(hDC, replaced);
    }
    DeleteDC(hDC);
  }
  HBITMAP copyBitmap(HBITMAP hBitmap)
  {
    HDC hdc = GetDC(GetConsoleWindow());
    HDC hdcDestination = CreateCompatibleDC(hdc);
    BITMAP bitmap;
    GetObject(hBitmap, sizeof(BITMAP), &bitmap);
    HBITMAP result = CreateCompatibleBitmap(hdc, bitmap.bmWidth, bitmap.bmHeight);
    HGDIOBJ dstold = SelectObject(hdcDestination, result);
    if (dstold)
    {
      drawBitmap(hdcDestination, 0, 0, hBitmap);
      SelectObject(hdcDestination, dstold);
    }
    DeleteDC(hdcDestination);
    ReleaseDC(GetConsoleWindow(), hdc);
    return result;
  }
}

