#include "stdafx.h"
#include "GlobalVariables.h"

double const PI = 3.141592;

int const MAX_FPS = 60;

int const MAP_SIZE = 15;
int const TILE_SIZE = 32;
int MAX_ENEMY_COUNT = 10;

int PLAYER_LIVES = 3;
int WALL_HP = 4;
int GOLD_HP = 1;

int const MSEC_IN_SEC = 1000;
int const SEC_IN_MIN = 60;
int const MKS_IN_SEC = 1000000;

int ENEMY_UPDATE_DELAY = 750;
int ENEMY_SHOOT_DELAY = 800;
int PLAYER_SHOOT_DELAY = 400;

double const DEFAULT_BULLET_SPEED = 0.14;
double const DEFAULT_OBJECT_SPEED = 0.07;
int VISIBLE_DISTANCE = 6;

bool DEBUG_DRAW_COLLISIONS = false;

int const KEY_UP = 72;
int const KEY_DOWN = 80;
int const KEY_LEFT = 75;
int const KEY_RIGHT = 77;

COLORREF WALL_COLOR = RGB(156, 74, 0);
COLORREF ALLY_COLOR = RGB(0, 128, 0);
COLORREF ENEMY_COLOR = RGB(128, 0, 0);

const std::string WALL_TEXTURE = "res/wall.bmp";
const std::string GOLD_TEXTURE = "res/treasure.bmp";
const std::string TANK_GREEN_1 = "res/tankgreen.bmp";
const std::string TANK_BLUE_1 = "res/tankblue.bmp";

POINT DEFAULT_DIRECTION = { 0, -1 };

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
INT64 getTimeMks(INT64 oldTimeMks)
{
  LARGE_INTEGER result;
  LARGE_INTEGER frequency;
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&result);
  result.QuadPart = result.QuadPart * MKS_IN_SEC / frequency.QuadPart;
  if (oldTimeMks)
    result.QuadPart -= oldTimeMks;
  return result.QuadPart;
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
  void rotateBitmap(HBITMAP hBitmap, const int degres, bool AdjustSize)
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
          Width = lround(bitmap.bmWidth * abs(Cosinus) + bitmap.bmHeight * abs(Sinus));
          Height = lround(bitmap.bmWidth * abs(Sinus) + bitmap.bmHeight * abs(Cosinus));
        }
        int OffsetX = lround((Width - Width * Cosinus + Height * Sinus) / 2);
        int OffsetY = lround((Height - Width * Sinus - Height * Cosinus) / 2);
        Points[0].x = (OffsetX);
        Points[0].y = (OffsetY);
        Points[1].x = lround(OffsetX + Width * Cosinus);
        Points[1].y = lround(OffsetY + Width * Sinus);
        Points[2].x = lround(OffsetX - Height * Sinus);
        Points[2].y = lround(OffsetY + Height * Cosinus);
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
          Width = lround(bitmap.bmWidth * abs(Cosinus) + bitmap.bmHeight * abs(Sinus));
          Height = lround(bitmap.bmWidth * abs(Sinus) + bitmap.bmHeight * abs(Cosinus));
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
      drawBitmap(hdcDestination, 0, 0, hBitmap, false);
      SelectObject(hdcDestination, dstold);
    }
    DeleteDC(hdcDestination);
    ReleaseDC(GetConsoleWindow(), hdc);
    return result;
  }
  PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
  {
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD    cClrBits;

    // Retrieve the bitmap color format, width, and height.  
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
      throw std::runtime_error("Get Object");

    // Convert the color format to a count of bits.  
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1)
      cClrBits = 1;
    else if (cClrBits <= 4)
      cClrBits = 4;
    else if (cClrBits <= 8)
      cClrBits = 8;
    else if (cClrBits <= 16)
      cClrBits = 16;
    else if (cClrBits <= 24)
      cClrBits = 24;
    else cClrBits = 32;

    // Allocate memory for the BITMAPINFO structure. (This structure  
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
    // data structures.)  

    if (cClrBits < 24)
      pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
        sizeof(BITMAPINFOHEADER) +
        sizeof(RGBQUAD) * (1 << cClrBits));

    // There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

    else
      pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
        sizeof(BITMAPINFOHEADER));

    // Initialize the fields in the BITMAPINFO structure.  

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
    if (cClrBits < 24)
      pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

    // If the bitmap is not compressed, set the BI_RGB flag.  
    pbmi->bmiHeader.biCompression = BI_RGB;

    // Compute the number of bytes in the array of color  
    // indices and store the result in biSizeImage.  
    // The width must be DWORD aligned unless the bitmap is RLE 
    // compressed. 
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
      * pbmi->bmiHeader.biHeight;
    // Set biClrImportant to 0, indicating that all of the  
    // device colors are important.  
    pbmi->bmiHeader.biClrImportant = 0;
    return pbmi;
  }

  void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,
    HBITMAP hBMP, HDC hDC)
  {
    HANDLE hf;                 // file handle  
    BITMAPFILEHEADER hdr;       // bitmap file-header  
    PBITMAPINFOHEADER pbih;     // bitmap info-header  
    LPBYTE lpBits;              // memory pointer  
    DWORD dwTotal;              // total count of bytes  
    DWORD cb;                   // incremental count of bytes  
    BYTE *hp;                   // byte pointer  
    DWORD dwTmp;

    pbih = (PBITMAPINFOHEADER)pbi;
    lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits)
      throw std::runtime_error("GlobalAlloc");

    // Retrieve the color table (RGBQUAD array) and the bits  
    // (array of palette indices) from the DIB.  
    if (!GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
      DIB_RGB_COLORS))
    {
      throw std::runtime_error("GetDIBits");
    }

    // Create the .BMP file.  
    hf = CreateFile(pszFile,
      GENERIC_READ | GENERIC_WRITE,
      (DWORD)0,
      NULL,
      CREATE_ALWAYS,
      FILE_ATTRIBUTE_NORMAL,
      (HANDLE)NULL);
    if (hf == INVALID_HANDLE_VALUE)
      throw std::runtime_error("CreateFile");
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
                                // Compute the size of the entire file.  
    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
      pbih->biSize + pbih->biClrUsed
      * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    // Compute the offset to the array of color indices.  
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
      pbih->biSize + pbih->biClrUsed
      * sizeof(RGBQUAD);

    // Copy the BITMAPFILEHEADER into the .BMP file.  
    if (!WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
      (LPDWORD)&dwTmp, NULL))
    {
      throw std::runtime_error("WriteFile");
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
    if (!WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
      + pbih->biClrUsed * sizeof(RGBQUAD),
      (LPDWORD)&dwTmp, (NULL)))
      throw std::runtime_error("WriteFile");

    // Copy the array of color indices into the .BMP file.  
    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;
    if (!WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL))
      throw std::runtime_error("WriteFile");

    // Close the .BMP file.  
    if (!CloseHandle(hf))
      throw std::runtime_error("CloseHandle");

    // Free memory.  
    GlobalFree((HGLOBAL)lpBits);
  }
}

