#include "stdafx.h"
#include "GraphicEngine.h"

#include "GlobalVariables.h"

#include <cassert>

const COLORREF COLOR_WHITE = 0xFFFFFF;
const COLORREF COLOR_GREY = 0x888888;
const COLORREF COLOR_GREEN = 0x00FF00;
const COLORREF COLOR_RED = 0x0000FF;
const COLORREF COLOR_BLUE = 0xFF0000;
const COLORREF COLOR_YELLOW = 0x00FFFF;
const COLORREF COLOR_BLACK = 0;
const COLORREF COLOR_SILVER = 0xC0C0C0;
/*
clBlack = TColor($000000);
clMaroon = TColor($000080);
clGreen = TColor($008000);
clOlive = TColor($008080);
clNavy = TColor($800000);
clPurple = TColor($800080);
clTeal = TColor($808000);
clGray = TColor($808080);
clSilver = TColor($C0C0C0);
clRed = TColor($0000FF);
clLime = TColor($00FF00);
clYellow = TColor($00FFFF);
clBlue = TColor($FF0000);
clFuchsia = TColor($FF00FF);
clAqua = TColor($FFFF00);
clLtGray = TColor($C0C0C0);
clDkGray = TColor($808080);
clWhite = TColor($FFFFFF);
*/

using namespace gdi;

gdi::GraphicEngine::GraphicEngine()
{
}

gdi::GraphicEngine::~GraphicEngine()
{
}

gdi::Canvas::Canvas(HDC hdc)
{
  /*
  HDC targetDC = hdc;
  if (targetDC == NULL)
  {
    targetDC = GetDC(NULL);
    hdc_ = CreateCompatibleDC(targetDC);
    ReleaseDC(NULL, targetDC);
  }
  else
    hdc_ = CreateCompatibleDC(targetDC);
  */
  hdc_ = CreateCompatibleDC(0);
  assert(hdc_ != NULL);
  pen.setHDC(hdc_);
  brush.setHDC(hdc_);
  font.setHDC(hdc_);
  //setBrush();
  //setPen();
  //setFont(COLOR_WHITE);
  setPenMode();
  SetStretchBltMode(hdc_, HALFTONE);
}

gdi::Canvas::~Canvas()
{
  reset();
  //DeleteObject(hPen_);
  //DeleteObject(hFont_);
  //DeleteObject(hBrush_);
  if (hdc_)
    DeleteDC(hdc_);
}

bool gdi::Canvas::drawTo(const HDC hdc, const int x, const int y) const
{
  return false;
}

bool gdi::Canvas::drawTo(const HBITMAP hBitmap, const int x, const int y) const
{
  return false;
}

HDC gdi::Canvas::getDC()const
{
  return hdc_;
}

WORD gdi::Canvas::getPenMode()const
{
  return penMode_;
}

HBITMAP gdi::Canvas::getBitmap() const
{
  return hBitmap_;
}

COLORREF gdi::Canvas::getBkColor()const
{
  return bkColor_;
}

void gdi::Canvas::setPenMode(WORD penMode)
{
  if (!hdc_)
    return;
  SetROP2(hdc_, penMode);
  penMode_ = penMode;
}

void gdi::Canvas::setTransparent(const bool transparent)
{
  if (!hdc_)
    return;
  if (transparent)
  {
    if (*brush.style == BS_SOLID)
    {
      setBkColor(*brush.color);
      setBkMode(OPAQUE);
    }
    else
    {
      setBkColor(!*brush.color);
      setBkMode(TRANSPARENT);
    }
  }
  else
  {
    setBkColor(0);
    setBkMode(OPAQUE);
  }
}

void gdi::Canvas::setBkColor(const COLORREF color)
{
  if (!hdc_)
    return;
  SetBkColor(hdc_, color);
  bkColor_ = color;
}

void gdi::Canvas::setBkMode(const int bkMode)
{
  if (!hdc_)
    return;
  SetBkMode(hdc_, bkMode);
  bkMode_ = bkMode;
}
void gdi::Canvas::setCopyMode(DWORD copyMode)
{
  copyMode_ = copyMode;
}
void gdi::Canvas::setBitmap(HBITMAP bitmap, bool deleteOldObject)
{
  if (!hdc_)
    return;
  tempBitmap_ = SelectObject(hdc_, bitmap);
  if (tempBitmap_)
  {
    if (deleteOldObject && hBitmap_)
      DeleteObject(hBitmap_);
    hBitmap_ = bitmap;
  }

}
void gdi::Canvas::reset()
{
  /*
  if (tempPen_)
    SelectObject(hdc_, tempPen_);
  if (tempBrush_)
    SelectObject(hdc_, tempBrush_);
  if (tempFont_)
    SelectObject(hdc_, tempFont_);
  t*/
  deselectBitmap();
}
void gdi::Canvas::deselectBitmap()
{
  if (tempBitmap_)
  {
    SelectObject(hdc_, tempBitmap_);
    tempBitmap_ = nullptr;
  }
}
/*
bool gdi::Canvas::draw(const Bitmap& bitmap, const int x, const int y)const
{
  if (!hdc_)
    return false;
  return draw(bitmap.getHandle(), x, y);
}

bool gdi::Canvas::draw(const HBITMAP hBitmap, const int x, const int y)const
{
if (!hdc_)
return false;
bool result = false;
if (hBitmap)
{
HDC hDC = CreateCompatibleDC(hdc_);
HGDIOBJ replaced = SelectObject(hDC, hBitmap);
if (replaced)
{
BITMAP bitmap;
GetObject(hBitmap, sizeof(BITMAP), &bitmap);
bool result = BitBlt(hdc_, x, y, bitmap.bmWidth, bitmap.bmHeight, hDC, 0, 0, copyMode);
SelectObject(hDC, replaced);
}
DeleteDC(hDC);
}
return result;
}
*/

bool gdi::Canvas::draw(const Bitmap& bitmap, const int x, const int y, const int width, const int height)const
{
  if (!hdc_)
    return false;
  return draw(bitmap.getHandle(), x, y, width, height);
}

bool gdi::Canvas::draw(const HBITMAP hBitmap, const POINT point)const
{
  if (!hdc_)
    return false;
  return draw(hBitmap, point.x, point.y);
}

bool gdi::Canvas::draw(const HBITMAP hBitmap, const int x, const int y, const int width, const int height)const
{
  if (!hdc_)
    return false;
  bool result = false;
  if (hBitmap)
  {
    HDC hDC = CreateCompatibleDC(hdc_);
    HGDIOBJ replaced = SelectObject(hDC, hBitmap);
    if (hDC && replaced)
    {
      BITMAP bitmap;
      GetObject(hBitmap, sizeof(BITMAP), &bitmap);
      if ((width == 0 || bitmap.bmWidth == width) && (height == 0 || bitmap.bmHeight == height))
        result = BitBlt(hdc_, x, y, bitmap.bmWidth, bitmap.bmHeight, hDC, 0, 0, copyMode_);
      else
        result = StretchBlt(hdc_, x, y, width, height, hDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, copyMode_);
      SelectObject(hDC, replaced);
    }
    DeleteDC(hDC);
  }
  return result;
}

bool gdi::Canvas::draw(const HBITMAP hBitmap, const RECT rect)const
{
  if (!hdc_)
    return false;
  return draw(hBitmap, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

bool gdi::Canvas::rotate(const HBITMAP hBitmap, const int degres, bool AdjustSize)const
{
  if (!hdc_)
    return false;
  if (degres % 360 == 0)
    return true;
  bool result = false;
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
      Points[0].x = lround(OffsetX);
      Points[0].y = lround(OffsetY);
      Points[1].x = lround(OffsetX + Width * Cosinus);
      Points[1].y = lround(OffsetY + Width * Sinus);
      Points[2].x = lround(OffsetX - Height * Sinus);
      Points[2].y = lround(OffsetY + Height * Cosinus);
      result = PlgBlt(hDC, Points, hDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, 0, 0, 0);
    }
    SelectObject(hDC, replaced);
  }
  DeleteDC(hDC);
  return result;
}

bool gdi::Canvas::rotate(const Bitmap & bitmap, const int degres, bool AdjustSize)const
{
  if (!hdc_)
    return false;
  bitmap.getHandle();
  return rotate(bitmap, degres, AdjustSize);
}

bool gdi::Canvas::moveTo(const POINT point)const
{
  if (!hdc_)
    return false;
  return MoveToEx(hdc_, point.x, point.y, NULL);
}

bool gdi::Canvas::moveTo(const int x, const int y)const
{
  if (!hdc_)
    return false;
  return MoveToEx(hdc_, x, y, NULL);
}

bool gdi::Canvas::lineTo(const int x, const int y)const
{
  if (!hdc_)
    return false;
  return LineTo(hdc_, x, y);
}

bool gdi::Canvas::lineTo(const int fromX, const int fromY, const int toX, const int toY)const
{
  if (!hdc_)
    return false;
  moveTo(fromX, fromY);
  return lineTo(toX, toY);
}

bool gdi::Canvas::lineTo(const POINT from, const POINT to)const
{
  if (!hdc_)
    return false;
  moveTo(from);
  return lineTo(to.x, to.y);
}

bool gdi::Canvas::rectangle(const int x1, const int y1, const int x2, const int y2)const
{
  if (!hdc_)
    return false;
  return Rectangle(hdc_, x1, y1, x2, y2);
}

bool gdi::Canvas::fillRect(const RECT rect)const
{
  if (!hdc_)
    return false;
  return Rectangle(hdc_, rect.left, rect.top, rect.right, rect.bottom);
}

bool gdi::Canvas::fillRect(const int x, const int y, const int width, const int height)const
{
  if (!hdc_)
    return false;
  RECT rect = { x, y, x + width, y + height };
  FillRect(hdc_, &rect, brush.getHandle());
  return true;
}

bool gdi::Canvas::textOut(std::string text, const int x, const int y)const
{
  if (!hdc_)
    return false;
  return TextOut(hdc_, x, y, text.c_str(), text.length());
}

bool gdi::Canvas::ellipse(const int x1, const int y1, const int x2, const int y2)const
{
  if (!hdc_)
    return false;
  return Ellipse(hdc_, x1, y1, x2, y2);
}

SIZE gdi::Canvas::getTextSize(const std::string text)const
{
  SIZE result;
  if (hdc_)
    GetTextExtentPoint32(hdc_, text.c_str(), text.length(), &result);
  return result;
}

HBITMAP getBitmap(BYTE* imageBuffer, int w, int h, HDC bitmapDC) {

  BITMAPINFO* info;
  info = (LPBITMAPINFO)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
  for (int i = 0; i < 255; i++)
  {
    info->bmiColors[i].rgbRed = (byte)i;
    info->bmiColors[i].rgbGreen = (byte)i;
    info->bmiColors[i].rgbBlue = (byte)i;
    info->bmiColors[i].rgbReserved = 0;
  }

  info->bmiHeader.biSize = sizeof(info->bmiHeader);
  info->bmiHeader.biWidth = w;
  info->bmiHeader.biHeight = h;
  info->bmiHeader.biPlanes = 1;
  info->bmiHeader.biBitCount = 8;
  info->bmiHeader.biCompression = BI_RGB;
  info->bmiHeader.biSizeImage = 0;
  info->bmiHeader.biClrUsed = 256;
  info->bmiHeader.biClrImportant = 0;

  HBITMAP bitmap = CreateCompatibleBitmap(bitmapDC, w, h);
  SetDIBits(bitmapDC, bitmap, 0, h, imageBuffer, info, DIB_RGB_COLORS);
  free(info);
  return bitmap;
}

gdi::Bitmap::Bitmap(WORD width, WORD height, WORD bitCount)
  :canvas(Canvas(GetDC(NULL)))
{
  HDC screenDC = GetDC(NULL);
  /*
  assert(screenDC != NULL);
  HDC newDC = CreateCompatibleDC(screenDC);
  assert(newDC != NULL);
  hBitmap_ = CreateCompatibleBitmap(screenDC, width, height);
  */
  bitmapInfo_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bitmapInfo_.bmiHeader.biWidth = width;
  bitmapInfo_.bmiHeader.biHeight = -height;
  bitmapInfo_.bmiHeader.biPlanes = 1;
  bitmapInfo_.bmiHeader.biBitCount = bitCount;
  bitmapInfo_.bmiHeader.biCompression = BI_RGB;
  bitmapInfo_.bmiHeader.biSizeImage = -1 * bitmapInfo_.bmiHeader.biHeight * bitmapInfo_.bmiHeader.biWidth * (bitCount >> 3);
  bitmapInfo_.bmiHeader.biClrUsed = 0;
  bitmapInfo_.bmiHeader.biClrImportant = 0;
  if (bitCount <= 16)
  {
    BITMAPINFO* pBitmapInfo = (BITMAPINFO*)alloca(offsetof(BITMAPINFO, bmiColors[256]));
    assert(pBitmapInfo != NULL);
    *pBitmapInfo = bitmapInfo_;
    if (bitCount <= 8)
    {
      pBitmapInfo->bmiHeader.biClrUsed = (1 << bitmapInfo_.bmiHeader.biBitCount);
      //grayscale
      for (int i = 0; i < pBitmapInfo->bmiHeader.biClrUsed - 1; ++i)
      {
        pBitmapInfo->bmiColors[i].rgbRed =
          pBitmapInfo->bmiColors[i].rgbGreen =
          pBitmapInfo->bmiColors[i].rgbBlue = (BYTE)(i*(255 / (pBitmapInfo->bmiHeader.biClrUsed - 1)));
        pBitmapInfo->bmiColors[i].rgbReserved = 0;
      }
    }
    else
    {
      //bitmapInfo_.bmiHeader.biCompression = BI_BITFIELDS;
      dib_.dsBitfields[0] = 0xF800;
      dib_.dsBitfields[1] = 0x07E0;
      dib_.dsBitfields[2] = 0x001F;
      //32bit
      //DIB.dsBitFields[0] : = 0x00FF0000;
      //DIB.dsBitFields[1] : = 0x0000FF00;
      //DIB.dsBitFields[2] : = 0x000000FF;
      pBitmapInfo->bmiColors[0].rgbBlue = GetRValue(dib_.dsBitfields[0]);
      pBitmapInfo->bmiColors[0].rgbGreen = GetGValue(dib_.dsBitfields[0]);
      pBitmapInfo->bmiColors[0].rgbRed = GetBValue(dib_.dsBitfields[0]);
      pBitmapInfo->bmiColors[1].rgbBlue = GetRValue(dib_.dsBitfields[1]);
      pBitmapInfo->bmiColors[1].rgbGreen = GetGValue(dib_.dsBitfields[1]);
      pBitmapInfo->bmiColors[1].rgbRed = GetBValue(dib_.dsBitfields[1]);
      pBitmapInfo->bmiColors[2].rgbBlue = GetRValue(dib_.dsBitfields[2]);
      pBitmapInfo->bmiColors[2].rgbGreen = GetGValue(dib_.dsBitfields[2]);
      pBitmapInfo->bmiColors[2].rgbRed = GetBValue(dib_.dsBitfields[2]);
      //pBitmapInfo->bmiColors[1].rgbBlue = dib_.dsBitfields[0];
      //pBitmapInfo->bmiColors[2].rgbGreen = dib_.dsBitfields[1];
      //pBitmapInfo->bmiColors[3].rgbRed = dib_.dsBitfields[2];
      //pBitmapInfo->bmiColors[4].rgbReserved = 0x00;
      //hPalette_ = CreateHalftonePalette(screenDC);
      //assert(hPalette_ != NULL);
    }
    hBitmap_ = CreateDIBSection(NULL, pBitmapInfo, DIB_RGB_COLORS, &bitData_, NULL, 0);
  }
  else
    hBitmap_ = CreateDIBSection(NULL, &bitmapInfo_, DIB_RGB_COLORS, &bitData_, NULL, 0);
  //dib_.dsBmih = bitmapInfo_.bmiHeader;

  /*
  if (hBitmap_)
  {
    HDC newDC = CreateCompatibleDC(screenDC);
    HGDIOBJ replaced = SelectObject(newDC, hBitmap_);
    PatBlt(newDC, 0, 0, width, height, WHITENESS);
    SelectObject(newDC, replaced);
    DeleteDC(newDC);
  }
  DeleteDC(newDC);
  */
  ReleaseDC(NULL, screenDC);
  assert(hBitmap_ != NULL);
  initialization();
}

gdi::Bitmap::Bitmap(HDC hdc, WORD width, WORD height)
  :canvas(Canvas(hdc))
{
  hBitmap_ = CreateCompatibleBitmap(hdc, width, height);
  assert(hBitmap_ != NULL);
  initialization();
}

bool gdi::Bitmap::loadFromFile(std::string path)
{
  bool result = false;
  free();
  hBitmap_ = (HBITMAP)LoadImage(NULL, path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);// 
  result = initialization();
  return result;
}

bool gdi::Bitmap::saveToFile(std::string path)
{
  HANDLE hFile;
  BITMAPFILEHEADER bitmapHeader;
  LPBYTE lpBits;
  DWORD dwTemp;

  lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, bitmapInfo_.bmiHeader.biSizeImage);

  if (!lpBits)
  {
    return false; // could not allocate bitmap
  }

  bitmapInfo_ = { 0 };
  bitmapInfo_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  if (!GetDIBits(canvas.getDC(), hBitmap_, 0, 0, NULL, (BITMAPINFO*)&bitmapInfo_, DIB_RGB_COLORS))
  {
    return false; // could not allocate bitmap
  }

  if (!GetDIBits(canvas.getDC(), hBitmap_, 0, bitmapInfo_.bmiHeader.biHeight, lpBits, &bitmapInfo_, DIB_RGB_COLORS))
  {
    return false; // could not allocate bitmap
  }

  if (bitmapInfo_.bmiHeader.biBitCount < 15)
  {
    bitmapInfo_.bmiHeader.biClrUsed = (1 << bitmapInfo_.bmiHeader.biBitCount);
  }

  hFile = CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
    NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hFile == INVALID_HANDLE_VALUE)
  {
    return false; // Could not open file
  }

  // type == BM
  bitmapHeader.bfType = 0x4d42;

  bitmapHeader.bfSize = (sizeof(BITMAPFILEHEADER) + bitmapInfo_.bmiHeader.biSize + bitmapInfo_.bmiHeader.biClrUsed * sizeof(RGBQUAD) + bitmapInfo_.bmiHeader.biSizeImage);
  bitmapHeader.bfReserved1 = 0;
  bitmapHeader.bfReserved2 = 0;

  bitmapHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + bitmapInfo_.bmiHeader.biSize + bitmapInfo_.bmiHeader.biClrUsed * sizeof(RGBQUAD);

  // write the bitmap file header to file
  WriteFile(hFile, (LPVOID)&bitmapHeader, sizeof(BITMAPFILEHEADER), &dwTemp, NULL);

  // write the bitmap header to file
  WriteFile(hFile, (LPVOID)&bitmapInfo_.bmiHeader, sizeof(BITMAPINFOHEADER) + bitmapInfo_.bmiHeader.biClrUsed * sizeof(RGBQUAD), &dwTemp, NULL);

  // copy the bitmap colour data into the file
  WriteFile(hFile, (LPSTR)lpBits, bitmapInfo_.bmiHeader.biSizeImage, &dwTemp, NULL);

  CloseHandle(hFile);

  GlobalFree((HGLOBAL)lpBits);
}

void gdi::Bitmap::setSize(WORD width, WORD height)
{
  if (width == 0 || height == 0)
    return;
  dib_ = { 0 };
  if (hBitmap_)
    GetObject(hBitmap_, sizeof(DIBSECTION), &dib_.dsBm);
  /*
  if (width!=dib_.dsBm.bmWidth || height != dib_.dsBm.bmHeight)
  {
    DIBSECTION dib = dib_;
    dib.dsBm.bmWidth = width;
    dib.dsBm.bmHeight = height;
    dib.dsBmih.biWidth = width;
    dib.dsBmih.biHeight = height;
  }
  */
  if (width != dib_.dsBm.bmWidth || height != dib_.dsBm.bmHeight)
  {
    hBitmap_ = (HBITMAP)CopyImage(hBitmap_, IMAGE_BITMAP, width, height, LR_COPYDELETEORG | LR_CREATEDIBSECTION);// 
    initialization();
  }
}

void gdi::Bitmap::setBitsPerPixel(const WORD bitCount)
{
  if (bitCount < 1 || bitCount>32)
    return;
  DIBSECTION dib = { 0 };
  HPALETTE pal = 0;
  HPALETTE newPal = 0;
  HDC dc = GetDC(0);
  HDC newDc = CreateCompatibleDC(dc);
  unsigned char* ppvBits;
  canvas.deselectBitmap();

  int  nColors = 1 << bitCount;
  
  GetObject(hBitmap_, sizeof(BITMAP), &dib_.dsBm);
  dib.dsBm = dib_.dsBm;
  dib.dsBm.bmBits = nullptr;
  dib.dsBmih.biWidth = dib.dsBm.bmWidth;
  dib.dsBmih.biHeight = dib.dsBm.bmHeight;
  dib.dsBmih.biPlanes = 1;
  dib.dsBmih.biBitCount = bitCount;
  dib.dsBmih.biCompression = BI_RGB;
  dib.dsBmih.biClrUsed = 0;
  dib.dsBmih.biClrUsed = (bitCount <= 8) ? nColors : 0;
  if (bitCount == 8)
  {
    //dc = GetDC(0);
    pal = CreateHalftonePalette(dc);
    //pal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
    /*
    LOGPALETTE lPal;
    lPal.palVersion = 0x300;
    lPal.palNumEntries = 16;
    int sysPalSize = GetDeviceCaps(dc, SIZEPALETTE);
    if (sysPalSize >= 16) 
    {
      GetSystemPaletteEntries(dc, 0, 8, &lPal.palPalEntry[0]);
      if ((COLORREF)(LPWORD)&lPal.palPalEntry[7] == COLOR_SILVER)
      {
        GetSystemPaletteEntries(dc, sysPalSize - 7, 7, &lPal.palPalEntry[9]);
      }
      GetSystemPaletteEntries(dc, sysPalSize - 8, 8, &lPal.palPalEntry[9]);
    }
    */
    //ReleaseDC(0, dc);
  }
  if (bitCount == 16)
  {
    pal = CreateHalftonePalette(dc);
    //dib.dsBmih.biCompression = BI_BITFIELDS;
    //dib.dsBitfields[0]  = 0xF800;
    //dib.dsBitfields[1]  = 0x07E0;
    //dib.dsBitfields[2]  = 0x001F;
  }

  UINT nSize = sizeof(BITMAPINFOHEADER);
  if (bitCount <= 8)
    nSize += +nColors * sizeof(RGBQUAD);
  BITMAPINFO* pBitmapInfo = (BITMAPINFO*)alloca(nSize);
  dib.dsBmih.biSize = sizeof(pBitmapInfo->bmiHeader);
  dib.dsBmih.biPlanes = 1;
  pBitmapInfo->bmiHeader = dib.dsBmih;
  dib.dsBm.bmWidth = dib.dsBmih.biWidth;
  dib.dsBm.bmHeight = dib.dsBmih.biHeight;
  HBITMAP newBitmap = nullptr;
  
  if (bitCount == 8)
  {
    int colorcount;
    //HGDIOBJ tempScr = SelectObject(newDc, hBitmap_);
    //colorcount = GetDIBColorTable(dc, 0, 256, pBitmapInfo->bmiColors);
    //SelectObject(newDc, tempScr);

    if (pal)
    {
      LOGPALETTE* lPal = (LOGPALETTE*)alloca(sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * 256);
      lPal->palVersion = 0x300;
      GetObject(pal, 4, &lPal->palNumEntries);
      GetPaletteEntries(pal, 0, lPal->palNumEntries, (LPPALETTEENTRY)lPal->palPalEntry);
      newPal = CreatePalette(lPal);

      colorcount = 0;
      GetObject(pal, sizeof(colorcount), &colorcount);
      GetPaletteEntries(pal, 0, colorcount, (LPPALETTEENTRY)pBitmapInfo->bmiColors);
      for (int i = 0; i < colorcount; i++)
      {
        std::swap(pBitmapInfo->bmiColors[i].rgbBlue, pBitmapInfo->bmiColors[i].rgbRed);
      }    
    }
    //LPWORD pIndex = (LPWORD)pBitmapInfo->bmiColors;
    //for (int i = 0; i < nColors; i++) *pIndex++ = i;
    newBitmap = CreateDIBSection(dc, pBitmapInfo, DIB_RGB_COLORS, (LPVOID*)&ppvBits, NULL, NULL);
  }
  else
  {
    /*
    pBitmapInfo->bmiColors[0].rgbBlue = GetRValue(dib_.dsBitfields[0]);
    pBitmapInfo->bmiColors[0].rgbGreen = GetGValue(dib_.dsBitfields[0]);
    pBitmapInfo->bmiColors[0].rgbRed = GetBValue(dib_.dsBitfields[0]);
    pBitmapInfo->bmiColors[0].rgbBlue = GetRValue(dib_.dsBitfields[0]);
    pBitmapInfo->bmiColors[0].rgbGreen = GetGValue(dib_.dsBitfields[0]);
    pBitmapInfo->bmiColors[0].rgbRed = GetBValue(dib_.dsBitfields[0]);
    pBitmapInfo->bmiColors[1].rgbBlue = GetRValue(dib_.dsBitfields[1]);
    pBitmapInfo->bmiColors[1].rgbGreen = GetGValue(dib_.dsBitfields[1]);
    pBitmapInfo->bmiColors[1].rgbRed = GetBValue(dib_.dsBitfields[1]);
    pBitmapInfo->bmiColors[2].rgbBlue = GetRValue(dib_.dsBitfields[2]);
    pBitmapInfo->bmiColors[2].rgbGreen = GetGValue(dib_.dsBitfields[2]);
    pBitmapInfo->bmiColors[2].rgbRed = GetBValue(dib_.dsBitfields[2]);
    */
    newBitmap = CreateDIBSection(dc, pBitmapInfo, DIB_RGB_COLORS, (LPVOID*)&ppvBits, NULL, NULL);
  }
  ///*
  if (pal)
  {
    HGDIOBJ tempScr = SelectObject(newDc, newBitmap);

    HPALETTE pal1 = SelectPalette(newDc, pal, FALSE);
    RealizePalette(newDc);
    PatBlt(newDc, 0, 0, dib.dsBm.bmWidth, dib.dsBm.bmHeight, WHITENESS);

    HDC oldDc = CreateCompatibleDC(dc);
    HGDIOBJ oldScr = SelectObject(oldDc, hBitmap_);
    HPALETTE pal2 = SelectPalette(oldDc, newPal, FALSE);
    RealizePalette(oldDc);
    BitBlt(newDc, 0, 0, dib.dsBm.bmWidth, dib.dsBm.bmHeight, oldDc, 0, 0, SRCCOPY);
    SelectPalette(oldDc, pal2, TRUE);
    SelectObject(oldDc, oldScr);
    DeleteDC(oldDc);

    SelectPalette(newDc, pal1, TRUE);
    SelectObject(newDc, tempScr);
  }
  //*/

  assert(ppvBits != NULL);
  GetDIBits(newDc, hBitmap_, 0, ::abs(dib.dsBmih.biHeight), ppvBits, pBitmapInfo, DIB_RGB_COLORS);
  drawBitmap(dc, 0, 0, hBitmap_);
  DeleteObject(hBitmap_);
  DeleteDC(newDc);
  drawBitmap(dc, 0, 0, newBitmap);
  ReleaseDC(0, dc);
  hBitmap_ = newBitmap;
  initialization();
}

gdi::Bitmap::~Bitmap()
{
  free();
}

HBITMAP gdi::Bitmap::getHandle() const
{
  return hBitmap_;
}

LONG gdi::Bitmap::getWidth() const
{
  return dib_.dsBm.bmWidth ? dib_.dsBm.bmWidth : dib_.dsBmih.biWidth;
}

LONG gdi::Bitmap::getHeight() const
{
  return dib_.dsBm.bmHeight ? dib_.dsBm.bmHeight : dib_.dsBmih.biHeight;
}

WORD gdi::Bitmap::getBitsPerPixel() const
{
  return dib_.dsBm.bmBitsPixel ? dib_.dsBm.bmBitsPixel : dib_.dsBmih.biBitCount;
}

bool gdi::Bitmap::initialization()
{
  bool result = false;
  assert(hBitmap_ != NULL);
  /*
  bitmap_ = { 0 };
  GetObject(hBitmap_, sizeof(BITMAP), &bitmap_);
  */
  //ZeroMemory(&dib_, sizeof(DIBSECTION));
  dib_ = { 0 };
  result = GetObject(hBitmap_, sizeof(DIBSECTION), &dib_);
  /*
  dib_.dsBmih.biSize = sizeof(BITMAPINFOHEADER);
  dib_.dsBmih.biWidth = dib_.dsBm.bmWidth;
  dib_.dsBmih.biHeight = dib_.dsBm.bmHeight;
  dib_.dsBmih.biPlanes = 1;
  dib_.dsBmih.biBitCount = dib_.dsBm.bmPlanes * dib_.dsBm.bmBitsPixel;
  //*/
  bitmapInfo_ = { 0 };
  bitmapInfo_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  GetDIBits(canvas.getDC(), hBitmap_, 0, 0, NULL, (BITMAPINFO*)&bitmapInfo_, DIB_RGB_COLORS);
  /*
  int nDepth = bitmapInfo_.bmiHeader.biBitCount;
  int nSize = bitmapInfo_.bmiHeader.biHeight * ((bitmapInfo_.bmiHeader.biWidth * (nDepth == 32 ? 4 : 3) + 3) & ~3);
  bitData_ = new BYTE[nSize];
  GetDIBits(canvas.getDC(), hBitmap_, 0, bitmapInfo_.bmiHeader.biHeight, bitData_, (BITMAPINFO*)&bitmapInfo_, DIB_RGB_COLORS);
  */
  if (canvas.getBitmap() != hBitmap_)
    canvas.setBitmap(hBitmap_);
  return result;
}

void gdi::Bitmap::free()
{
  if (hBitmap_)
  {
    DeleteObject(hBitmap_);
    hBitmap_ = nullptr;
  }
  //if (palette_)
  //  DeleteObject(palette_);
  //if (dibHandle_)
  //  DeleteObject(dibHandle_);
}

gdi::Pallete::Pallete(int nEntries)
{
  if (nEntries <= 256)
  {
    paletteInfo_.palVersion = 0x0300;
    paletteInfo_.palNumEntries = nEntries;
    ::memset(paletteInfo_.palPalEntry, 0, nEntries * sizeof(PALETTEENTRY));
    for (int i = 0; i < nEntries; i++)
    {
      paletteInfo_.palPalEntry[i].peRed =
        paletteInfo_.palPalEntry[i].peGreen =
        paletteInfo_.palPalEntry[i].peBlue = (BYTE)(i*(255 / (nEntries - 1)));
    }
    palette_ = CreatePalette(&paletteInfo_);
    free(&paletteInfo_.palPalEntry);
  }
  else
  {
    HDC screenDC = GetDC(NULL);
    palette_ = CreateHalftonePalette(screenDC);
    ReleaseDC(NULL, screenDC);
  }
}

gdi::Pallete::Pallete(HPALETTE palette)
{
  int paletteSize = 0;
  if (palette != 0 && GetObject(palette, sizeof(paletteSize), &paletteSize) != 0 && paletteSize != 0)
  {
    paletteInfo_.palVersion = 0x0300;
    paletteInfo_.palNumEntries = paletteSize;
    GetPaletteEntries(palette, 0, paletteSize, paletteInfo_.palPalEntry);
    palette_ = CreatePalette(&paletteInfo_);
  }
}

gdi::Pallete::~Pallete()
{
  if (palette_)
    DeleteObject(palette_);
}

HPALETTE gdi::Pallete::getPallete()
{
  return palette_;
}

void gdi::Pen::setPen(const COLORREF color, const UINT style, const UINT width)
{
  if (!hdc_)
    return;
  pen_.lopnStyle = style;
  pen_.lopnColor = color;
  pen_.lopnWidth.x = width;
  free();
  hPen_ = CreatePenIndirect(&pen_);
  tempPen_ = SelectObject(hdc_, hPen_);
}
void gdi::Pen::setPen(LOGPEN pen, bool deleteOldObject)
{
  if (!hdc_)
    return;
  HPEN newhPen = CreatePenIndirect(&pen);
  tempPen_ = SelectObject(hdc_, newhPen);
  if (newhPen && tempPen_)
  {
    pen_ = pen;
    if (deleteOldObject)
      free();
    hPen_ = newhPen;
  }
}
void gdi::Pen::setPen(HPEN pen, bool deleteOldObject)
{
  if (!hdc_)
    return;
  tempPen_ = SelectObject(hdc_, pen);
  if (tempPen_)
  {
    if (deleteOldObject)
      free();
    hPen_ = pen;
  }
}
void gdi::Pen::setPenStyle(const UINT style)
{
  if (!hdc_)
    return;
  pen_.lopnStyle = style;
  setPen(pen_);
}
void gdi::Pen::setPenColor(const COLORREF color)
{
  if (!hdc_)
    return;
  pen_.lopnColor = color;
  setPen(pen_);
}
void gdi::Pen::setPenWidth(const UINT width)
{
  if (!hdc_)
    return;
  pen_.lopnWidth.x = width;
  setPen(pen_);
}
bool gdi::Pen::setHDC(HDC hdc)
{
  if (hdc && !hdc_)
  {
    hdc_ = hdc;
    if (!hPen_)
      setPen();
    return true;
  }
  return false;
}
bool gdi::Pen::free()
{
  if (hPen_)
  {
    bool result = DeleteObject(hPen_);
    hPen_ = nullptr;
    return result;
  }
  return false;
}
bool gdi::Pen::reset()
{
  if (tempPen_)
  {
    bool result = SelectObject(hdc_, tempPen_);
    tempPen_ = nullptr;
    return result;
  }
  return false;
}
LOGPEN gdi::Pen::getPen()const
{
  return pen_;
}
gdi::Pen::Pen(HDC hdc, const COLORREF color, const UINT width, const UINT style)
{
  //assert(hdc != NULL);
  hdc_ = hdc;
  setPen(color, style, width);
}
gdi::Pen::~Pen()
{
  reset();
  free();
}
HPEN gdi::Pen::getHandle()const
{
  return hPen_;
}
UINT gdi::Pen::getPenStyle()const
{
  return pen_.lopnStyle;
}
UINT gdi::Pen::getPenWidth()const
{
  return pen_.lopnWidth.x;
}
COLORREF gdi::Pen::getPenColor()const
{
  return pen_.lopnColor;
}


gdi::Brush::Brush(HDC hdc, const COLORREF color, const UINT style, const ULONG_PTR hatch)
{
  //assert(hdc != NULL);
  hdc_ = hdc;
  setBrush(color, style, hatch);
  update();
}
gdi::Brush::~Brush()
{
  reset();
  free();
}
HBRUSH gdi::Brush::getHandle()const
{
  return hBrush_;
}
bool gdi::Brush::free()
{
  if (hBrush_)
  {
    bool result = DeleteObject(hBrush_);
    hBrush_ = nullptr;
    return result;
  }
  return false;
}
bool gdi::Brush::reset()
{
  if (tempBrush_)
  {
    bool result = SelectObject(hdc_, tempBrush_);
    tempBrush_ = nullptr;
    return result;
  }
  return false;
}

bool gdi::Brush::setHDC(HDC hdc)
{
  if (hdc && !hdc_)
  {
    hdc_ = hdc;
    if (!hBrush_)
      setBrush();
    return true;
  }
  return false;
}

void gdi::Brush::update()
{
  color = &brush_.lbColor;
  style = &brush_.lbStyle;
  hatch = &brush_.lbHatch;
}

LOGBRUSH gdi::Brush::getBrush()const
{
  return brush_;
}
UINT gdi::Brush::getBrushStyle()const
{
  return brush_.lbStyle;
}
COLORREF gdi::Brush::getBrushColor()const
{
  return brush_.lbColor;
}

void gdi::Brush::setBrush(const COLORREF color, const UINT style, const ULONG_PTR hatch)
{
  if (!hdc_)
    return;
  brush_.lbStyle = style;
  brush_.lbColor = color;
  brush_.lbHatch = hatch;
  setBrush(brush_);
}
void gdi::Brush::setBrush(LOGBRUSH brush, bool deleteOldObject)
{
  if (!hdc_)
    return;
  if (brush.lbStyle == BS_SOLID)
  {
    SetBkColor(hdc_, brush.lbColor);
    SetBkMode(hdc_, OPAQUE);
  }
  else
  {
    SetBkColor(hdc_, !brush.lbColor);
    SetBkMode(hdc_, TRANSPARENT);
  }
  HBRUSH newhBrush = CreateBrushIndirect(&brush);
  tempBrush_ = SelectObject(hdc_, newhBrush);
  if (newhBrush && tempBrush_)
  {
    brush_ = brush;
    if (deleteOldObject)
      free();
    hBrush_ = newhBrush;
  }
}
void gdi::Brush::setBrush(HBRUSH brush, bool deleteOldObject)
{
  if (!hdc_)
    return;
  tempBrush_ = SelectObject(hdc_, brush);
  if (tempBrush_)
  {
    if (deleteOldObject)
      free();
    hBrush_ = brush;
  }
}
void gdi::Brush::setBrushStyle(const UINT style)
{
  if (!hdc_)
    return;
  brush_.lbStyle = style;
  setBrush(brush_);
}
void gdi::Brush::setBrushColor(const COLORREF color)
{
  if (!hdc_)
    return;
  brush_.lbColor = color;
  setBrush(brush_);
}

bool gdi::Font::free()
{
  if (hFont_)
  {
    bool result = DeleteObject(hFont_);
    hFont_ = nullptr;
    return result;
  }
  return false;
}

bool gdi::Font::reset()
{
  if (tempFont_)
  {
    bool result = SelectObject(hdc_, tempFont_);
    tempFont_ = nullptr;
    return result;
  }
  return false;
}

bool gdi::Font::setHDC(HDC hdc)
{
  if (hdc && !hdc_)
  {
    hdc_ = hdc;
    if (!hFont_)
      setFont(fontColor_);
    return true;
  }
  return false;
}

gdi::Font::Font(HDC hdc)
{
  //assert(hdc != NULL);
  hdc_ = hdc;
}

gdi::Font::~Font()
{
  reset();
  free();
}

HFONT gdi::Font::getHandle()const
{
  return hFont_;
}

SIZE gdi::Font::getTextSize(const std::string text)const
{
  SIZE result;
  if (hdc_)
    GetTextExtentPoint32(hdc_, text.c_str(), text.length(), &result);
  return result;
}

LOGFONT gdi::Font::getFont()const
{
  return font_;
}

COLORREF gdi::Font::getFontColor()const
{
  return fontColor_;
}

void gdi::Font::setFont(LOGFONT font, bool deleteOldObject)
{
  if (!hdc_)
    return;

  HFONT newhFont = CreateFontIndirect(&font);
  tempFont_ = SelectObject(hdc_, newhFont);
  if (newhFont && tempFont_)
  {
    font_ = font;
    if (deleteOldObject)
      free();
    hFont_ = newhFont;
  }
}

void gdi::Font::setFont(HFONT font, bool deleteOldObject)
{
  if (!hdc_)
    return;
  tempFont_ = SelectObject(hdc_, font);
  if (tempFont_)
  {
    if (deleteOldObject)
      free();
    hFont_ = font;
  }
}

void gdi::Font::setFont(const COLORREF color, const LONG height, const LONG weight, const bool italic, const bool underline, const BYTE quality, const LPCSTR fontName)
{
  if (!hdc_)
    return;
  setFontColor(color);
  setFont(height, weight, italic, underline, quality, fontName);
}

void gdi::Font::setFont(const LONG height, const LONG weight, const bool italic, const bool underline, const BYTE quality, const LPCSTR fontName)
{
  if (!hdc_)
    return;
  font_.lfHeight = height;
  font_.lfWidth = 0;
  font_.lfEscapement = 0;
  font_.lfOrientation = 0;
  font_.lfWeight = weight;
  font_.lfItalic = italic ? TRUE : FALSE;
  font_.lfUnderline = underline ? TRUE : FALSE;
  font_.lfStrikeOut = FALSE;
  font_.lfCharSet = DEFAULT_CHARSET;
  font_.lfOutPrecision = OUT_DEFAULT_PRECIS;
  font_.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  font_.lfQuality = quality;
  font_.lfPitchAndFamily = DEFAULT_PITCH;
  strcpy_s(font_.lfFaceName, fontName);

  hFont_ = CreateFont(height, 0, 0, 0, weight, font_.lfItalic, font_.lfUnderline, FALSE,
    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, quality, DEFAULT_PITCH, fontName);
}

void gdi::Font::setFontColor(const COLORREF color)
{
  if (!hdc_)
    return;
  SetTextColor(hdc_, color);
  fontColor_ = color;
}

void gdi::Font::setFontHeight(const LONG height)
{
  if (!hdc_)
    return;
  font_.lfHeight = height;
  setFont(font_);
}

void gdi::Font::setFontName(const LPCSTR fontName)
{
  if (!hdc_)
    return;
  strcpy_s(font_.lfFaceName, fontName);
  setFont(font_);
}

void gdi::Font::setFontStyle(const bool bold, const bool italic, const bool underline)
{
  if (!hdc_)
    return;
  font_.lfWeight = bold ? FW_BOLD : FW_NORMAL;
  font_.lfItalic = italic ? TRUE : FALSE;
  font_.lfUnderline = underline ? TRUE : FALSE;
  setFont(font_);
}

void gdi::Font::setFontQuality(const BYTE quality)
{
  if (!hdc_)
    return;
  font_.lfQuality = quality;
  setFont(font_);
}
