#ifndef GRAPHICENGINE_H
#define GRAPHICENGINE_H

extern const COLORREF COLOR_WHITE;
extern const COLORREF COLOR_GREY;
extern const COLORREF COLOR_GREEN;
extern const COLORREF COLOR_RED;
extern const COLORREF COLOR_BLUE;
extern const COLORREF COLOR_YELLOW;
extern const COLORREF COLOR_BLACK;

namespace gdi
{
  class Bitmap;
  class Pen;
  class Brush;
  class Font;
  //
  class Canvas
  {
  public:
    Canvas(HDC hdc = NULL);
    ~Canvas();
    bool drawTo(const HDC hdc, const int x = 0, const int y = 0)const;
    bool drawTo(const HBITMAP hBitmap, const int x = 0, const int y = 0)const;

    //bool draw(const Bitmap& bitmap, const int x, const int y)const;
    bool draw(const Bitmap& bitmap, const int x = 0, const int y = 0, const int width = 0, const int height = 0)const;
    //bool draw(const HBITMAP hBitmap, const int x, const int y)const;
    bool draw(const HBITMAP hBitmap, const POINT point)const;
    bool draw(const HBITMAP hBitmap, const int x = 0, const int y = 0, const int width = 0, const int height = 0)const;
    bool draw(const HBITMAP hBitmap, const RECT rect)const;
    bool rotate(const HBITMAP hBitmap, const int degres, bool AdjustSize = false)const;
    bool rotate(const Bitmap& bitmap, const int degres, bool AdjustSize = false)const;
    bool moveTo(const POINT point)const;
    bool moveTo(const int x, const int y)const;
    bool lineTo(const int x, const int y)const;
    bool lineTo(const int fromX, const int fromY, const int toX, const int toY)const;
    bool lineTo(const POINT from, const POINT to)const;
    bool rectangle(const int x1, const int y1, const int x2, const int y2)const;
    bool fillRect(const RECT rect)const;
    bool fillRect(const int x, const int y, const int width, const int height)const;
    bool textOut(std::string text, const int x, const int y)const;
    bool ellipse(const int x1, const int y1, const  int x2, const int y2)const;
    HDC getDC()const;
    SIZE getTextSize(const std::string text)const;
    WORD getPenMode()const;
    LOGPEN getPen()const;
    HBITMAP getBitmap()const;
    LOGBRUSH getBrush()const;
    UINT setBrushStyle()const;
    UINT setPenStyle()const;
    UINT getPenWidth()const;
    LOGFONT getFont()const;
    COLORREF getFontColor()const;
    COLORREF getBkColor()const;
    COLORREF getBrushColor()const;
    COLORREF getPenColor()const;
    void setBitmap(HBITMAP bitmap, bool deleteOldObject = true);
    void setBrush(LOGBRUSH brush, bool deleteOldObject = true);
    void setBrush(HBRUSH brush, bool deleteOldObject = true);
    void setPen(LOGPEN pen, bool deleteOldObject = true);
    void setPen(HPEN pen, bool deleteOldObject = true);
    void setFont(LOGFONT font, bool deleteOldObject = true);
    void setFont(HFONT font, bool deleteOldObject = true);
    //Create new Brush with following parameters
    //BS_SOLID, BS_NULL, BS_HOLLOW, BS_HATCHED, BS_PATTERN, BS_PATTERN8X8, BS_DIBPATTERN, BS_DIBPATTERN8X8, BS_DIBPATTERNPT 
    //HS_HORIZONTAL, HS_VERTICAL, HS_FDIAGONAL, HS_BDIAGONAL, HS_CROSS, HS_DIAGCROSS
    void setBrush(const COLORREF color = COLOR_BLACK, const UINT style = BS_SOLID, const ULONG_PTR hatch = HS_HORIZONTAL);
    //BS_SOLID, BS_NULL, BS_HOLLOW, BS_HATCHED, BS_PATTERN, BS_PATTERN8X8, BS_DIBPATTERN, BS_DIBPATTERN8X8, BS_DIBPATTERNPT 
    void setBrushStyle(const UINT style = BS_SOLID);
    void setBrushColor(const COLORREF color);
    //Create new Pen with following parameters
    //psSolid, psDash, psDot, psDashDot, psDashDotDot, psClear, psInsideFrame, psUserStyle, psAlternate 
    void setPen(const COLORREF color = COLOR_WHITE, const UINT style = PS_SOLID, const UINT width = 1);
    //psSolid, psDash, psDot, psDashDot, psDashDotDot, psClear, psInsideFrame, psUserStyle, psAlternate 
    void setPenStyle(const UINT style = PS_SOLID);
    void setPenColor(const COLORREF color);
    void setPenWidth(const UINT width = 1);
    //Create new Fone with following parameters
    void setFont(const COLORREF color, const LONG height = -11, const LONG weight = 400, const bool italic = false,
      const bool underline = false, const BYTE quality = DEFAULT_QUALITY, const LPCSTR fontName = "Tahoma");
    //Create new Fone with following parameters
    void setFont(const LONG height = -11, const LONG weight = 400, const bool italic = false,
      const bool underline = false, const BYTE quality = DEFAULT_QUALITY, const LPCSTR fontName = "Tahoma");
    void setFontColor(const COLORREF color);
    void setFontHeight(const LONG height = -11);
    void setFontName(const LPCSTR fontName = "Tahoma");
    void setFontStyle(const bool bold = false, const bool italic = false, const bool underline = false);
    //DEFAULT_QUALITY, DRAFT_QUALITY, PROOF_QUALITY, NONANTIALIASED_QUALITY, ANTIALIASED_QUALITY, CLEARTYPE_QUALITY, CLEARTYPE_NATURAL_QUALITY
    void setFontQuality(const BYTE quality = DEFAULT_QUALITY);
    //R2_BLACK, R2_WHITE, R2_NOP, R2_NOT, R2_COPYPEN, R2_NOTCOPYPEN, R2_MERGEPENNOT,
    //R2_MASKPENNOT, R2_MERGENOTPEN, R2_MASKNOTPEN, R2_MERGEPEN, R2_NOTMERGEPEN,
    //R2_MASKPEN, R2_NOTMASKPEN, R2_XORPEN, R2_NOTXORPEN
    void setPenMode(WORD penMode = R2_COPYPEN);
    void setTransparent(const bool transparent = true);
    void setBkColor(const COLORREF color);
    //TRANSPARENT, OPAQUE
    void setBkMode(const int bkMode = TRANSPARENT);
    //SRCCOPY, SRCPAINT, SRCAND, SRCINVERT, SRCERASE, NOTSRCCOPY, NOTSRCERASE, 
    //MERGECOPY, MERGEPAINT, PATCOPY, PATPAINT, PATINVERT, 
    //DSTINVERT, BLACKNESS, WHITENESS
    void setCopyMode(DWORD copyMode);
    void reset();
    void deselectBitmap();
  protected:
  private:
    HDC hdc_ = nullptr;
    int bkMode_ = TRANSPARENT;
    DWORD copyMode_ = SRCCOPY;
    WORD penMode_ = R2_COPYPEN;
    LOGPEN pen_;
    HPEN hPen_ = nullptr;
    LOGBRUSH brush_;
    HBRUSH hBrush_ = nullptr;
    LOGFONT font_;
    HFONT hFont_ = nullptr;
    COLORREF fontColor_ = COLOR_WHITE;
    HBITMAP hBitmap_ = nullptr;
    HGDIOBJ tempPen_ = nullptr;
    HGDIOBJ tempBrush_ = nullptr;
    HGDIOBJ tempFont_ = nullptr;
    HGDIOBJ tempBitmap_ = nullptr;
    COLORREF bkColor_ = 0;
  };

  class Pallete
  {
  public:
    Pallete();
    Pallete(HPALETTE palette);
  private:
    LOGPALETTE paletteInfo_;
    PALETTEENTRY arPalEntries[255];
    HPALETTE palette_ = nullptr;
  };

  //Image based class prepared for draw
  class Bitmap
  {
  public:
    Bitmap(WORD width = 0, WORD height =0, WORD bitCount = 16);
    Bitmap(HDC hdc, WORD width, WORD height);
    bool loadFromFile(std::string path);
    bool saveToFile(std::string path);
    void setSize(WORD width, WORD height);
    ~Bitmap();
    Canvas canvas;
    HBITMAP getHandle() const;
    LONG getWidth() const;
    LONG getHeight() const;
    WORD getBitsPerPixel() const;
  private:
    bool initialization();
    void free();
    //
    HBITMAP hBitmap_ = nullptr;
    HBITMAP oldhBitmap_ = nullptr;
    BITMAP bitmap_ = { 0 };
    BITMAPINFO bitmapInfo_ = { 0 };
    DIBSECTION dib_ = { 0 };
    VOID* bitData_;
  };

  class Sprite : public Bitmap
  {

  };

  class GraphicEngine
  {
  public:
    GraphicEngine();
    ~GraphicEngine();
  };
}
#endif //GRAPHICENGINE_H