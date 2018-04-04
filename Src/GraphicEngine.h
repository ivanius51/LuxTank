#ifndef GRAPHICENGINE_H
#define GRAPHICENGINE_H

extern const COLORREF COLOR_WHITE;
extern const COLORREF COLOR_GREY;
extern const COLORREF COLOR_GREEN;
extern const COLORREF COLOR_RED;
extern const COLORREF COLOR_BLUE;
extern const COLORREF COLOR_YELLOW;
extern const COLORREF COLOR_BLACK;
extern const COLORREF COLOR_SILVER;
extern const COLORREF COLOR_MAROON;
extern const COLORREF COLOR_OLIVE;
extern const COLORREF COLOR_NAVY;
extern const COLORREF COLOR_PURPLE;
extern const COLORREF COLOR_TEAL;
extern const COLORREF COLOR_LIME;
extern const COLORREF COLOR_FUCHSIA;
extern const COLORREF COLOR_AQUA;

namespace gdi
{
  //forcedeclare
  class Bitmap;

  bool prepareBitmapAlpha(HDC hdc, HBITMAP hBmp);
  bool scale(const HBITMAP hBitmap, const int percent);
  bool scale(const Bitmap& bitmap, const int percent);
  bool rotate(const HBITMAP hBitmap, const int degres, bool AdjustSize = false);
  bool rotate(const Bitmap& bitmap, const int degres, bool AdjustSize = false);
  bool draw(const HDC hdc, const HBITMAP hBitmap, const int x = 0, const int y = 0, const int width = 0, const int height = 0, const bool stretch = true, const DWORD copymode = SRCCOPY);
  bool draw(const HBITMAP hBitmap, const HDC hdc, const int x = 0, const int y = 0, const int width = 0, const int height = 0, const bool stretch = true, const DWORD copymode = SRCCOPY);
  HBITMAP copyBitmap(HBITMAP hBitmap, bool deleteOriginal = false);

  class Pen
  {
  public:
    Pen(HDC hdc = NULL, const COLORREF color = COLOR_WHITE, const UINT width = 1, const UINT style = PS_SOLID);
    ~Pen();

    HPEN getHandle()const;

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

    bool setHDC(HDC hdc);

    bool free();
    bool reset();

    const COLORREF* color;
    const UINT* style;
    const LONG* width;
  private:
    void setValues();

    HDC hdc_ = nullptr;
    LOGPEN pen_ = { 0 };
    HPEN hPen_ = nullptr;
    HGDIOBJ tempPen_ = nullptr;
  };
  class Brush
  {
  public:
    Brush(HDC hdc = NULL, const COLORREF color = COLOR_BLACK, const UINT style = BS_SOLID, const ULONG_PTR hatch = HS_HORIZONTAL);
    ~Brush();

    HBRUSH getHandle()const;

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
    bool setHDC(HDC hdc);

    const COLORREF* color;
    const UINT* style;
    const ULONG_PTR* hatch;
  private:
    void setValues();
    HDC hdc_ = nullptr;
    LOGBRUSH brush_ = { 0 };
    HBRUSH hBrush_ = nullptr;
    HGDIOBJ tempBrush_ = nullptr;
  };
  class Font
  {
  public:
    Font(HDC hdc = NULL);
    ~Font();

    HFONT getHandle()const;

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
    bool setHDC(HDC hdc);


  private:
    void setValues();
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

    bool drawTo(const HDC hdc, const int offsetX = 0, const int OffsetY = 0, const int width = 0, const int height = 0, const int x = 0, const int y = 0)const;
    bool drawTo(const HBITMAP hBitmap, const int x = 0, const int y = 0, const int width = 0, const int height = 0, const bool stretch = true)const;
    //bool draw(const Bitmap& bitmap, const int x, const int y)const;
    bool draw(const Bitmap& bitmap, const int x = 0, const int y = 0, const int width = 0, const int height = 0)const;
    bool drawOffset(const Bitmap& bitmap, const int offsetX = 0, const int OffsetY = 0, const int width = 0, const int height = 0)const;
    //bool draw(const HBITMAP hBitmap, const int x, const int y)const;
    bool draw(const HBITMAP hBitmap, const POINT point)const;
    bool draw(const HBITMAP hBitmap, const int x = 0, const int y = 0, const int width = 0, const int height = 0, const bool stretch = true)const;
    bool draw(const HBITMAP hBitmap, const RECT rect)const;
    bool scale(const int percent)const;
    bool rotate(const int degres, bool AdjustSize = false)const;
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

    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
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
    Pallete(int nEntries = 256);
    Pallete(HPALETTE palette);
    ~Pallete();
    HPALETTE getPallete();
  private:
    LOGPALETTE paletteInfo_;
    PALETTEENTRY arPalEntries[255];
    HPALETTE palette_ = nullptr;
  };

  //Image based class prepared for draw
  class Bitmap
  {
  public:
    Bitmap(const WORD width = 0, const  WORD height = 0, const  WORD bitCount = 24);
    Bitmap(const HDC hdc, const  WORD width, const  WORD height);
    Bitmap(const std::string path);
    ~Bitmap();

    bool loadFromFile(std::string path);
    bool saveToFile(std::string path);
    void setSize(WORD width, WORD height, bool reDraw = false);
    void scale(WORD percent);
    bool setTransparent32Bit();
    bool drawTo(HBITMAP hBitmap);

    Canvas canvas;
    HBITMAP getHandle() const;
    LONG getWidth() const;
    LONG getHeight() const;
    WORD getBitsPerPixel() const;
    bool isSelected() const;
    COLORREF transparentColor() const;

    void free();
  protected:
    //not work good
    void setBitsPerPixel(const WORD bitCount);
    bool refresh();
  private:
    COLORREF transparent_;
    HBITMAP hBitmap_ = nullptr;
    HBITMAP oldhBitmap_ = nullptr;
    HPALETTE hPalette_ = nullptr;
    BITMAPINFO bitmapInfo_ = { 0 };
    DIBSECTION dib_ = { 0 };
    VOID* bitData_;
  };

  class Sprite
  {
  public:
    Sprite(const std::string path, const WORD size);
    Sprite(const Bitmap& source, const WORD size);
    Bitmap storage;
    Bitmap currentImage;

    WORD getCount();
    WORD getRowCount();
    WORD getColumnCount();

    void scale(WORD percent);
    void setSize(const WORD size);
    bool setCell(const WORD index);
    bool setCell(const WORD nRow, const WORD nColumn);
  private:
    WORD size_ = 0;
    int cellIndex_ = -1;

  };

  class AnimatedSprite : public Sprite
  {
  public:
    AnimatedSprite(const std::string path, const WORD size, WORD lastFrame = 0, WORD fps = 0, WORD scaled = 100);
    AnimatedSprite(const Bitmap& source, const WORD size, WORD lastFrame = 0, WORD fps = 0, WORD scaled = 100);
    bool update(const double elapsed);
    unsigned char getLoopType()const;
    void setOneWayFlow(bool oneWay = true);
    void setFPS(const WORD fps = 0);
    void setFrame(const WORD frame = 0);
    void setLastFrame(const WORD lastFrame = 0);
    bool isLoop()const;
    bool isOneWayFlow()const;
  private:
    bool forwardWay_ = true;
    bool oneWayFlow_ = true;
    bool loop_ = true;
    double frameTime_ = 0;
    WORD frameDelay_ = 1000;//fps = 1
    WORD currentFrame_ = 0;
    WORD lastFrame_ = 0;
  };

  class GraphicEngine
  {
  public:
    GraphicEngine();
    ~GraphicEngine();
  };
}
#endif //GRAPHICENGINE_H