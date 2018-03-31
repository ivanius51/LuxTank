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
  //forcedeclare
  class Bitmap;

  class Pen
  {
  public:
    Pen(HDC hdc, const COLORREF color = COLOR_WHITE, const UINT width = 1, const UINT style = PS_SOLID);
    ~Pen();
    //psSolid, psDash, psDot, psDashDot, psDashDotDot, psClear, psInsideFrame, psUserStyle, psAlternate 
    UINT getPenStyle()const;
    UINT getPenWidth()const;
    COLORREF getPenColor()const;
    LOGPEN getPen()const;
    //Assign new Pen
    void setPen(LOGPEN pen, bool deleteOldObject = true);
    //Assign new Pen
    void setPen(HPEN pen, bool deleteOldObject = true);
    //Create new Pen with following parameters
    //psSolid, psDash, psDot, psDashDot, psDashDotDot, psClear, psInsideFrame, psUserStyle, psAlternate 
    void setPen(const COLORREF color = COLOR_WHITE, const UINT style = PS_SOLID, const UINT width = 1);
    //psSolid, psDash, psDot, psDashDot, psDashDotDot, psClear, psInsideFrame, psUserStyle, psAlternate 
    void setPenStyle(const UINT style = PS_SOLID);
    void setPenColor(const COLORREF color);
    void setPenWidth(const UINT width = 1);

    bool free();
    bool reset();
  private:
    HDC hdc_ = nullptr;
    LOGPEN pen_ = { 0 };
    HPEN hPen_ = nullptr;
    HGDIOBJ tempPen_ = nullptr;
  };
  class Brush
  {
  public:
    Brush(HDC hdc, const COLORREF color = COLOR_BLACK, const UINT style = BS_SOLID, const ULONG_PTR hatch = HS_HORIZONTAL);
    ~Brush();
    COLORREF getBrushColor()const;
    LOGBRUSH getBrush()const;
    //BS_SOLID, BS_NULL, BS_HOLLOW, BS_HATCHED, BS_PATTERN, BS_PATTERN8X8, BS_DIBPATTERN, BS_DIBPATTERN8X8, BS_DIBPATTERNPT 
    UINT getBrushStyle()const;
    //Assign to new brush
    void setBrush(LOGBRUSH brush, bool deleteOldObject = true);
    //Assign to new brush
    void setBrush(HBRUSH brush, bool deleteOldObject = true);
    //Create new Brush with following parameters
    //BS_SOLID, BS_NULL, BS_HOLLOW, BS_HATCHED, BS_PATTERN, BS_PATTERN8X8, BS_DIBPATTERN, BS_DIBPATTERN8X8, BS_DIBPATTERNPT 
    //HS_HORIZONTAL, HS_VERTICAL, HS_FDIAGONAL, HS_BDIAGONAL, HS_CROSS, HS_DIAGCROSS
    void setBrush(const COLORREF color = COLOR_BLACK, const UINT style = BS_SOLID, const ULONG_PTR hatch = HS_HORIZONTAL);
    //BS_SOLID, BS_NULL, BS_HOLLOW, BS_HATCHED, BS_PATTERN, BS_PATTERN8X8, BS_DIBPATTERN, BS_DIBPATTERN8X8, BS_DIBPATTERNPT 
    void setBrushStyle(const UINT style = BS_SOLID);
    void setBrushColor(const COLORREF color);

    bool free();
    bool reset();
  private:
    HDC hdc_ = nullptr;
    LOGBRUSH brush_ = {0};
    HBRUSH hBrush_ = nullptr;
    HGDIOBJ tempBrush_ = nullptr;
  };
  class Font
  {
  public:
    Font(HDC hdc);
    ~Font();

    SIZE getTextSize(const std::string text)const;
    LOGFONT getFont()const;
    COLORREF getFontColor()const;
    //Assign to new Font
    void setFont(LOGFONT font, bool deleteOldObject = true);
    //Assign to new Font
    void setFont(HFONT font, bool deleteOldObject = true);
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

    bool free();
    bool reset();
  private:
    HDC hdc_ = nullptr;
    LOGFONT font_;
    HFONT hFont_ = nullptr;
    COLORREF fontColor_ = COLOR_WHITE;
    HGDIOBJ tempFont_ = nullptr;
  };
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

    Font font;
    Pen pen;
    Brush brush;
    HDC getDC()const;
    HBITMAP getBitmap()const;
    WORD getPenMode()const;
    COLORREF getBkColor()const;
    SIZE getTextSize(const std::string text)const;

    void setBitmap(HBITMAP bitmap, bool deleteOldObject = true);
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
    
    HBITMAP hBitmap_ = nullptr;
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
    Bitmap(WORD width = 0, WORD height = 0, WORD bitCount = 16);
    Bitmap(HDC hdc, WORD width, WORD height);
    ~Bitmap();

    bool loadFromFile(std::string path);
    bool saveToFile(std::string path);
    void setSize(WORD width, WORD height);
    
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