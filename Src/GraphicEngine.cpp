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

using namespace gdi;

gdi::GraphicEngine::GraphicEngine()
{
}


gdi::GraphicEngine::~GraphicEngine()
{
}

gdi::Canvas::Canvas()
{
  hdc_ = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
  assert(hdc_ != NULL);
  Canvas(hdc_);
}

gdi::Canvas::Canvas(HDC hdc)
{
  hdc_ = hdc;
  setBrush();
  setPen();
  setFont(COLOR_WHITE);
  setPenMode();
  SetStretchBltMode(hdc_, HALFTONE);
}

gdi::Canvas::~Canvas()
{
  SelectObject(hdc_, tempPen_);
  SelectObject(hdc_, tempBrush_);
  SelectObject(hdc_, tempFont_);
  DeleteObject(hPen_);
  DeleteObject(hFont_);
  DeleteObject(hBrush_);
}

HDC gdi::Canvas::getDC()const
{
  return hdc_;
}

WORD gdi::Canvas::getPenMode()const
{
  return penMode_;
}

LOGPEN gdi::Canvas::getPen()const
{
  return pen_;
}

LOGBRUSH gdi::Canvas::getBrush()const
{
  return brush_;
}

UINT gdi::Canvas::setBrushStyle()const
{
  return brush_.lbStyle;
}

UINT gdi::Canvas::setPenStyle()const
{
  return pen_.lopnStyle;
}

UINT gdi::Canvas::getPenWidth()const
{
  return pen_.lopnWidth.x;
}

LOGFONT gdi::Canvas::getFont()const
{
  return font_;
}

COLORREF gdi::Canvas::getFontColor()const
{
  return fontColor_;
}

COLORREF gdi::Canvas::getBkColor()const
{
  return bkColor_;
}

COLORREF gdi::Canvas::getBrushColor()const
{
  return brush_.lbColor;
}

COLORREF gdi::Canvas::getPenColor()const
{
  return pen_.lopnColor;
}

void gdi::Canvas::setBrush(const COLORREF color, const UINT style, const ULONG_PTR hatch)
{
  if (!hdc_)
    return;
  brush_.lbStyle = style;
  brush_.lbColor = color;
  brush_.lbHatch = hatch;
  setBrush(brush_);
}

void gdi::Canvas::setBrush(LOGBRUSH brush)
{
  if (!hdc_)
    return;
  if (brush.lbStyle == BS_SOLID)
  {
    setBkColor(brush.lbColor);
    setBkMode(OPAQUE);
  }
  else
  {
    setBkColor(!brush.lbColor);
    setBkMode(TRANSPARENT);
  }
  brush_ = brush;
  if (hBrush_)
    DeleteObject(hBrush_);
  hBrush_ = CreateBrushIndirect(&brush_);
  tempBrush_ = SelectObject(hdc_, hBrush_);
}

void gdi::Canvas::setBrushStyle(const UINT style)
{
  if (!hdc_)
    return;
  brush_.lbStyle = style;
  setBrush(brush_);
}

void gdi::Canvas::setBrushColor(const COLORREF color)
{
  if (!hdc_)
    return;
  brush_.lbColor = color;
  setBrush(brush_);
}

void gdi::Canvas::setPen(const COLORREF color, const UINT style, const UINT width)
{
  if (!hdc_)
    return;
  pen_.lopnStyle = style;
  pen_.lopnColor = color;
  pen_.lopnWidth.x = width;
  if (hPen_)
    DeleteObject(hPen_);
  hPen_ = CreatePenIndirect(&pen_);
  tempPen_ = SelectObject(hdc_, hPen_);
}

void gdi::Canvas::setPen(LOGPEN pen)
{
  if (!hdc_)
    return;
  pen_ = pen;
  if (hPen_)
    DeleteObject(hPen_);
  hPen_ = CreatePenIndirect(&pen_);
  tempPen_ = SelectObject(hdc_, hPen_);
}

void gdi::Canvas::setPenStyle(const UINT style)
{
  if (!hdc_)
    return;
  pen_.lopnStyle = style;
  setPen(pen_);
}

void gdi::Canvas::setPenColor(const COLORREF color)
{
  if (!hdc_)
    return;
  pen_.lopnColor = color;
  setPen(pen_);
}

void gdi::Canvas::setPenWidth(const UINT width)
{
  if (!hdc_)
    return;
  pen_.lopnWidth.x = width;
  setPen(pen_);
}

void gdi::Canvas::setFont(LOGFONT font)
{
  if (!hdc_)
    return;
  font_ = font;
  if (hFont_)
    DeleteObject(hFont_);
  hFont_ = CreateFontIndirect(&font_);
  tempFont_ = SelectObject(hdc_, hFont_);
}

void gdi::Canvas::setFont(const COLORREF color, const LONG height, const LONG weight, const bool italic, const bool underline, const BYTE quality, const LPCSTR fontName)
{
  if (!hdc_)
    return;
  setFontColor(color);
  setFont(height, weight, italic, underline, quality, fontName);
}

void gdi::Canvas::setFont(const LONG height, const LONG weight, const bool italic, const bool underline, const BYTE quality, const LPCSTR fontName)
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

void gdi::Canvas::setFontColor(const COLORREF color)
{
  if (!hdc_)
    return;
  SetTextColor(hdc_, color);
  fontColor_ = color;
}

void gdi::Canvas::setFontHeight(const LONG height)
{
  if (!hdc_)
    return;
  font_.lfHeight = height;
  setFont(font_);
}

void gdi::Canvas::setFontName(const LPCSTR fontName)
{
  if (!hdc_)
    return;
  strcpy_s(font_.lfFaceName, fontName);
  setFont(font_);
}

void gdi::Canvas::setFontStyle(const bool bold, const bool italic, const bool underline)
{
  if (!hdc_)
    return;
  font_.lfWeight = bold ? FW_BOLD : FW_NORMAL;
  font_.lfItalic = italic ? TRUE : FALSE;
  font_.lfUnderline = underline ? TRUE : FALSE;
  setFont(font_);
}

void gdi::Canvas::setFontQuality(const BYTE quality)
{
  if (!hdc_)
    return;
  font_.lfQuality = quality;
  setFont(font_);
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
    if (brush_.lbStyle == BS_SOLID)
    {
      setBkColor(brush_.lbColor);
      setBkMode(OPAQUE);
    }
    else
    {
      setBkColor(!brush_.lbColor);
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

bool gdi::Canvas::draw(const Bitmap& bitmap, const int x, const int y)const
{
  if (!hdc_)
    return false;
  return draw(bitmap.getHandle(), x, y);
}

bool gdi::Canvas::draw(const Bitmap& bitmap, const int x, const int y, const int width, const int height)const
{
  if (!hdc_)
    return false;
  return draw(bitmap.getHandle(), x, y, width, height);
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
      bool result = BitBlt(hdc_, x, y, bitmap.bmWidth, bitmap.bmHeight, hDC, 0, 0, SRCCOPY);
      SelectObject(hDC, replaced);
    }
    DeleteDC(hDC);
  }
  return result;
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
    if (replaced)
    {
      BITMAP bitmap;
      GetObject(hBitmap, sizeof(BITMAP), &bitmap);
      if (bitmap.bmWidth == width && bitmap.bmHeight == height)
        result = BitBlt(hdc_, x, y, bitmap.bmWidth, bitmap.bmHeight, hDC, 0, 0, SRCCOPY);
      else
        result = StretchBlt(hdc_, x, y, width, height, hDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
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
  FillRect(hdc_, &rect, hBrush_);
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

gdi::Bitmap::Bitmap(WORD width, WORD height)
{
  initialization(CreateCompatibleBitmap(CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL), width, height));
}

gdi::Bitmap::Bitmap(HDC hdc, WORD width, WORD height)
{
  canvas = Canvas(hdc);
  initialization(CreateCompatibleBitmap(hdc, width, height));
}

gdi::Bitmap::Bitmap(HBITMAP hBitmap)
{
  initialization(hBitmap);
}

void gdi::Bitmap::initialization(HBITMAP hBitmap)
{
  handle_ = hBitmap;
  assert(handle_ != NULL);
  GetObject(handle_, sizeof(DIBSECTION), &dib_);
  dib_.dsBmih.biSize = sizeof(BITMAPINFOHEADER);
  dib_.dsBmih.biWidth = dib_.dsBm.bmWidth;
  dib_.dsBmih.biHeight = dib_.dsBm.bmHeight;
  dib_.dsBmih.biPlanes = 1;
  dib_.dsBmih.biBitCount = dib_.dsBm.bmPlanes * dib_.dsBm.bmBitsPixel;
  
  //GetObject(handle_, sizeof(BITMAP), &bitmap_);
}

gdi::Bitmap::~Bitmap()
{
  if (handle_)
    DeleteObject(handle_);
  if (palette_)
    DeleteObject(palette_);
  if (dibHandle_)
    DeleteObject(dibHandle_);
}

HBITMAP gdi::Bitmap::getHandle() const
{
  return handle_;
}
