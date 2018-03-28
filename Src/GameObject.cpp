#include "stdafx.h"
#include "GameObject.h"

#include "Game.h"

GameObject::GameObject(POINT position, UINT hp, int attackDamage)
{
  hp_ = hp;
  attackDamage_ = attackDamage;
  position_ = position;
}

GameObject::GameObject(int x, int y, UINT hp, int attackDamage)
{
  hp_ = hp;
  maxHp_ = hp;
  attackDamage_ = attackDamage;
  position_.x = x;
  position_.y = y;
}

GameObject::~GameObject()
{
}

POINT GameObject::getPosition()
{
  return position_;
}

POINT GameObject::getDirection()
{
  return direction_;
}
POINT GameObject::getOldDirection()
{
  return oldDirection_;
}

void GameObject::setOldDirection()
{
  oldDirection_ = direction_;
}

UINT GameObject::getHP()
{
  return hp_;
}

UINT GameObject::getMaxHP()
{
  return maxHp_;
}

int GameObject::getAttackDamage()
{
  return attackDamage_;
}

bool GameObject::canMove()
{
  return canMove_;
}

bool GameObject::isWalkable()
{
  return isWalkable_;
}

bool GameObject::isDead()
{
  return hp_ <= 0;
}

void GameObject::takeDamage(int damage)
{
  hp_ -= damage;
}

void GameObject::bindUpdateCallback(std::function<void(GameObject&)> function)
{
  updateCallbacks_.push_back(std::bind(function, std::placeholders::_1));
}

void GameObject::updateCallback(GameObject& gameobject)
{
  for (const auto& callback : updateCallbacks_)
    callback(gameobject);
}

void GameObject::updateCallback()
{
  GameObject::updateCallback(*this);
}

bool GameObject::isSamePosition(const POINT & point)
{
  return point.y == position_.y && point.x == position_.x;
}

void GameObject::setPosition(int x, int y)
{
  position_.x = x;
  position_.y = y;
}

void GameObject::setPosition(POINT point)
{
  position_ = point;
}

void GameObject::setDirection(int x, int y)
{
  oldDirection_ = direction_;
  direction_.x = x;
  direction_.y = y;
}

void GameObject::setDirection(POINT point)
{
  setDirection(point.x, point.y);
}

void GameObject::setHP(UINT hp)
{
  hp_ = hp;
}

void GameObject::setAttackDamage(int damage)
{
  attackDamage_ = damage;
}

void GameObject::setCanMove(bool move)
{
  canMove_ = move;
}

void GameObject::setIsWalkable(bool walkable)
{
  isWalkable_ = walkable;
}

VisualObject::VisualObject(int x, int y, UINT hp, int attackDamage, COLORREF color, COLORREF background, UINT width, UINT height)
  :GameObject(x, y, hp, attackDamage)
{
  targetdc_ = 0;// Game::instance().getHdc();
  targeBitmap_ = 0;//Game::instance().getHdc();
  color_ = color;
  background_ = background;
  if (width)
    width_ = width;
  else
    width_ = Game::instance().getWorld().getTileSize();
  if (height)
    height_ = height;
  else
    height_ = Game::instance().getWorld().getTileSize();

  brush_ = CreateSolidBrush(background_);
  gizmoBrush_ = gdi::createBrush(BS_HATCHED, background_, HS_DIAGCROSS);
  pen_ = CreatePen(PS_SOLID, 1, color_);
}

VisualObject::VisualObject(int x, int y, UINT hp, int attackDamage, COLORREF color, COLORREF background, const std::string & texture, UINT width, UINT height)
  :VisualObject(x, y, hp, attackDamage, color, background, width, height)
{
  if (texture != "")
    texture_ = static_cast<HBITMAP>(LoadImage(NULL, texture.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
}

VisualObject::~VisualObject()
{
  if (texture_)
    DeleteObject(texture_);
  if (brush_)
    DeleteObject(brush_);
  if (pen_)
    DeleteObject(pen_);
}

void VisualObject::draw()
{
  if (targetdc_ != 0 && targeBitmap_ != 0)
    drawTo(targetdc_, targeBitmap_);
  else
    drawTo(Game::instance().getBufferDc(), Game::instance().getBuffer());
}

COLORREF VisualObject::getColor()
{
  return color_;
}

COLORREF VisualObject::getBackground()
{
  return background_;
}

UINT VisualObject::getWidth()
{
  return width_;
}

UINT VisualObject::getHeight()
{
  return height_;
}

POINT VisualObject::getOffset()
{
  return offset_;
}

HBITMAP VisualObject::getTexture()
{
  return texture_;
}


void VisualObject::takeDamage(int damage, POINT damageDirection)
{
  GameObject::takeDamage(damage);
  if (damageDirection.x == 1)
    Destrucions.left++;
  if (damageDirection.x == -1)
    Destrucions.right++;
  if (damageDirection.y == 1)
    Destrucions.top++;
  if (damageDirection.y == -1)
    Destrucions.bottom++;
}

bool VisualObject::beginDraw(HDC hdc, HBITMAP hbitmap, bool trasparent)
{
  HGDIOBJ replaced = SelectObject(hdc, hbitmap);
  if (replaced)
  {
    targetdc_ = hdc;
    targeBitmap_ = hbitmap;

    tempBrush_ = SelectObject(hdc, brush_);

    if (!trasparent)
    {
      SetBkColor(hdc, background_);
      SetBkMode(hdc, OPAQUE);
    }
    else
    {
      SetBkColor(hdc, !background_);
      SetBkMode(hdc, TRANSPARENT);
    }

    //Pen paint style
    tempPen_ = SelectObject(hdc, pen_);
    //R2_BLACK, R2_WHITE, R2_NOP, R2_NOT, R2_COPYPEN, R2_NOTCOPYPEN, R2_MERGEPENNOT,
    //R2_MASKPENNOT, R2_MERGENOTPEN, R2_MASKNOTPEN, R2_MERGEPEN, R2_NOTMERGEPEN,
    //R2_MASKPEN, R2_NOTMASKPEN, R2_XORPEN, R2_NOTXORPEN
    SetROP2(hdc, R2_COPYPEN);

    return true;
  }
  return false;
}

void VisualObject::endDraw()
{
  if (targetdc_ != 0)
  {
    VisualObject::drawGizmo();
    if (tempBrush_)
      SelectObject(targetdc_, tempBrush_);
    if (tempPen_)
      SelectObject(targetdc_, tempPen_);
  }
}

void VisualObject::drawGizmo()
{
  if (DEBUG_DRAW_COLLISIONS)
  {
    POINT Position = getScreenPositionCenter();

    HGDIOBJ old = SelectObject(targetdc_, gizmoBrush_);

    //Ellipse collision TODO: add box(square) collision
    //now only max size for collision
    int maxsize = std::max(getWidth(), getHeight());
    Ellipse(targetdc_,
      Position.x - maxsize / 2 - 1, Position.y - maxsize / 2 - 1,
      Position.x + maxsize / 2 + 1, Position.y + maxsize / 2 + 1);

    SelectObject(targetdc_, old);
  }
}

void VisualObject::changeBrush(UINT style, COLORREF color, ULONG_PTR hatch)
{
  if (brush_)
  {
    DeleteObject(brush_);
    brush_ = nullptr;
  }
  if (!brush_)
    brush_ = gdi::createBrush(style, color, hatch);
}

void VisualObject::changePen(UINT style, COLORREF color, UINT width)
{
  if (pen_)
  {
    DeleteObject(pen_);
    pen_ = nullptr;
  }
  if (!pen_)
    pen_ = gdi::createPen(style, width, color);
}

void VisualObject::setOffset(int x, int y)
{
  offset_.x = x;
  offset_.y = y;
}
void VisualObject::setOffset(POINT point)
{
  offset_ = point;
}
void VisualObject::setWidth(UINT width)
{
  width_ = width;
}
void VisualObject::setHeight(UINT height)
{
  height_ = height;
}
POINT VisualObject::getScreenPosition()
{
  UINT TileSize = Game::instance().getWorld().getTileSize();
  return POINT({ int(getPosition().x * TileSize + getOffset().x), int(getPosition().y * TileSize + getOffset().y) });
}
POINT VisualObject::getScreenPositionCenter()
{
  UINT TileSize = Game::instance().getWorld().getTileSize();
  return POINT({ int(getPosition().x * TileSize + TileSize / 2 + getOffset().x), int(getPosition().y * TileSize + TileSize / 2 + getOffset().y) });
}

Wall::Wall(int x, int y, UINT hp, int attackDamage, COLORREF color, COLORREF background, UINT width, UINT height)
  :VisualObject(x, y, hp, attackDamage, color, background, "", width, height)
{
  setIsWalkable(false);
  setCanMove(false);
}

Wall::Wall(POINT point, UINT hp, int attackDamage, COLORREF color, COLORREF background, UINT width, UINT height)
  : Wall(point.x, point.y, hp, attackDamage, color, background, width, height)
{
}

Wall::Wall(int x, int y, UINT hp, int attackDamage, const std::string & texture, COLORREF color, UINT width, UINT height)
  : VisualObject(x, y, hp, attackDamage, COLOR_GREY, color, texture, width, height)
{
  setIsWalkable(false);
  setCanMove(false);
}

Wall::Wall(POINT point, UINT hp, int attackDamage, const std::string & texture, COLORREF color, UINT width, UINT height)
  : Wall(point.x, point.y, hp, attackDamage, texture, color, width, height)
{
}

void Wall::draw()
{
  VisualObject::draw();
}

void Wall::drawTo(HDC hdc, HBITMAP hbitmap)
{
  if (isDead())
    return;
  if (VisualObject::beginDraw(hdc, hbitmap))
  {
    UINT TileSize = Game::instance().getWorld().getTileSize();
    POINT Position = getScreenPosition();
    int stage = 0;
    if (getHP() < getMaxHP())
      stage = int(TileSize / (getMaxHP() - 1)) - 1;

    if (getTexture())
    {
      gdi::drawBitmap(hdc, Position.x, Position.y, TileSize, TileSize, getTexture());
      //destuctions 
      BitBlt(hdc, Position.x, Position.y, Destrucions.left * stage, TileSize, hdc, 0, 0, BLACKNESS);
      BitBlt(hdc, Position.x, Position.y, TileSize, Destrucions.top * stage, hdc, 0, 0, BLACKNESS);
      BitBlt(hdc, Position.x + TileSize - Destrucions.right * stage, Position.y, Destrucions.right * stage, TileSize, hdc, 0, 0, BLACKNESS);
      BitBlt(hdc, Position.x, Position.y + TileSize - Destrucions.bottom * stage, TileSize, Destrucions.bottom * stage, hdc, 0, 0, BLACKNESS);
    }
    else
    {
      Rectangle(hdc,
        Position.x, Position.y,
        Position.x + TileSize, Position.y + TileSize);

      SelectObject(hdc, GetStockObject(DC_BRUSH));
      SetDCBrushColor(hdc, getBackground());

      Rectangle(hdc,
        Position.x + Destrucions.left * stage, Position.y + Destrucions.top * stage,
        Position.x + TileSize - Destrucions.right * stage, Position.y + TileSize - Destrucions.bottom * stage);

    }
    VisualObject::endDraw();
  };
}

void Wall::update(double elapsed)
{
  if (isDead())
  {
    Game::instance().getWorld().deleteObject(this);
  }
  
  double frameSpeed = elapsed * DEFAULT_BULLET_SPEED + modf(frameTime_, &frameTime_);
  frameTime_ = frameSpeed;
  //updateCallback();
}

MovableObject::MovableObject(int x, int y, COLORREF color, COLORREF background, UINT hp, int attackDamage, UINT width, UINT height)
  :VisualObject(x, y, hp, attackDamage, color, background, width, height)
{
  isMooving_ = false;
  setIsWalkable(false);
  setCanMove(true);
}

MovableObject::MovableObject(int x, int y, UINT hp, int attackDamage, const std::string & texture, COLORREF color, COLORREF background, UINT width, UINT height)
  : VisualObject(x, y, hp, attackDamage, color, background, texture, width, height)
{
  isMooving_ = false;
  setIsWalkable(false);
  setCanMove(true);
}

UINT MovableObject::getUpdateTime()
{
  return updateTime_;
}
UINT MovableObject::getUpdateDelay()
{
  return updateDelay_;
}
void MovableObject::setUpdateDelay(UINT updateDelay)
{
  updateDelay_ = updateDelay;
}

bool MovableObject::canUpdate()
{
  bool result = ((GetTickCount() - updateTime_) > updateDelay_);
  if (result)
    updateTime_ = GetTickCount();
  return result;
}

void MovableObject::rotate(POINT point)
{
  if (!isMooving_)
    setDirection(point);
}

void MovableObject::rotateTo(POINT point)
{
  if (!isMooving_)
    setDirection(sign(point.x - getPosition().x), sign(point.y - getPosition().y));
}

void MovableObject::moveForward()
{
  isMooving_ = true;
}

void MovableObject::stop()
{
  isMooving_ = false;
}

bool MovableObject::isMooving()
{
  return isMooving_;
}


Tank::Tank(int x, int y, COLORREF color, COLORREF background, UINT hp, int attackDamage, UINT width, UINT height)
  :MovableObject(x, y, color, background, hp, attackDamage, width, height)
{
  setIsWalkable(false);
  setCanMove(true);
}

Tank::Tank(POINT point, COLORREF color, COLORREF background, UINT hp, int attackDamage, UINT width, UINT height)
  : Tank(point.x, point.y, color, background, hp, attackDamage, width, height)
{
}

Tank::Tank(int x, int y, const std::string & texture, COLORREF color, UINT hp, int attackDamage, UINT width, UINT height)
  : MovableObject(x, y, hp, attackDamage, texture, color, color, width, height)
{
  setIsWalkable(false);
  setCanMove(true);
}

Tank::Tank(POINT point, const std::string & texture, COLORREF color, UINT hp, int attackDamage, UINT width, UINT height)
  : Tank(point.x, point.y, texture, color, hp, attackDamage, width, height)
{
}

bool Tank::isEnemy()
{
  return isenemy_;
}

bool Tank::isEnemy(GameObject* gameobject)
{
  if (dynamic_cast<Tank*>(gameobject))
    return this->isEnemy() != dynamic_cast<Tank*>(gameobject)->isEnemy();
  else
    return false;// !gameobject || !gameobject->isWalkable();
}

void Tank::setEnemy(bool enemy)
{
  isenemy_ = enemy;
}

bool Tank::isPlayer()
{
  return isplayer_;
}

void Tank::setPlayer(bool player)
{
  isplayer_ = player;
}

void Tank::setShootDelay(UINT shootDelay)
{
  shootDelay_ = shootDelay;
}

void Tank::shoot()
{
  if ((GetTickCount() - shootTime_) > shootDelay_)
  {
    shootTime_ = GetTickCount();
    Game::instance().addBullet(new Bullet(this));
  }
}

void Tank::draw()
{
  MovableObject::draw();
}

void Tank::drawTo(HDC hdc, HBITMAP hbitmap)
{
  if (isDead())
    return;
  if (VisualObject::beginDraw(hdc, hbitmap))
  {
    UINT TileSize = Game::instance().getWorld().getTileSize();
    POINT offset = getOffset();
    POINT Position = getScreenPosition();

    if (getTexture())
    {
      HBITMAP texture = gdi::copyBitmap(getTexture());
      int degres = 0;
      if (getDirection().x == -1)
        degres = 270;
      else
        if (getDirection().y == 1)
          degres = 180;
        else
          if (getDirection().x == 1)
            degres = 90;
      gdi::rotateTexture(texture, degres);
      gdi::drawBitmap(hdc, Position.x, Position.y, TileSize, TileSize, texture, true);
      DeleteObject(texture);
    }
    else
    {
      int width = int(TileSize / 5);
      int height = int(TileSize / 3);
      int center = int(TileSize / 2);

      Rectangle(hdc,
        Position.x + center - width * ((getDirection().x < 0) ? 1 : 2), Position.y + center - width * ((getDirection().y < 0) ? 1 : 2),
        Position.x + center + width * ((getDirection().x > 0) ? 1 : 2), Position.y + center + width * ((getDirection().y > 0) ? 1 : 2));

      Ellipse(hdc,
        Position.x + center - width, Position.y + center - width,
        Position.x + center + width, Position.y + center + width);

      width = 2;
      if (getDirection().x == 0)
        Rectangle(hdc,
          Position.x + center - 1, Position.y + center - height * ((getDirection().y < 0) ? 0 : -1),
          Position.x + center + 1, Position.y + center + height * ((getDirection().y > 0) ? 0 : -1));
      else
        Rectangle(hdc,
          Position.x + center - height * ((getDirection().x < 0) ? 0 : -1), Position.y + center + 1,
          Position.x + center + height * ((getDirection().x > 0) ? 0 : -1), Position.y + center - 1);

    }
    VisualObject::endDraw();
  };
}

void Tank::update(double elapsed)
{
  if (isDead())
    Game::instance().getWorld().deleteObject(this);
  
  double frameSpeed = elapsed * DEFAULT_OBJECT_SPEED + modf(frameTime_, &frameTime_);
  frameTime_ = frameSpeed;

  int TileSize = Game::instance().getWorld().getTileSize();
  int MapSize = Game::instance().getWorld().getMapSize();

  POINT NewPosition = getPosition();
  POINT Direction = getDirection();
  NewPosition.x += Direction.x;
  NewPosition.y += Direction.y;

  if (isMooving())
  {
    if ((abs(getOffset().x) >= TileSize - 1) || (abs(getOffset().y) >= TileSize - 1))
    {
      stop();
      setOffset(0, 0);
      if (Game::instance().getWorld().canMoveTo(NewPosition))
        setPosition(NewPosition);
    }
    else
    {
      setOffset(getOffset().x + int(Direction.x * frameSpeed), getOffset().y + int(Direction.y * frameSpeed));
      if (!Game::instance().getWorld().canMoveTo(NewPosition) && (abs(getOffset().x) >= (TileSize / 5) || abs(getOffset().y) >= (TileSize / 5)))
      {
        stop();
        setOffset(0, 0);
      }
    }
  }

  //TODO: Enemy Actions move it to some AI class.method
  if (!isPlayer() && canUpdate())
  {
    //if AI - find player, gold and move to it or if see it - shoot
    GameObject* Player = Game::instance().getWorld().getPlayer();

    //see player
    if (Game::instance().getWorld().isInVisibleDistance(getPosition(), Player->getPosition()))
    {
      //can turn and shoot
      if (Game::instance().getWorld().isIntersection(getPosition(), Player->getPosition()))
      {
        rotateTo(Player->getPosition());
        this->shoot();
      }
      else
      {
        NewPosition = getPosition();
        if (rand() % 100 < 50)
          NewPosition.x += sign(Player->getPosition().x - NewPosition.x);
        else
          NewPosition.y += sign(Player->getPosition().y - NewPosition.y);
        rotateTo(NewPosition);
        if (Game::instance().getWorld().canMoveTo(NewPosition))
          moveForward();
        else
          this->shoot();
      }
    }
    if (!isMooving())
    {
      if (Game::instance().getWorld().canMoveTo(NewPosition))
      {
        moveForward();
        if (rand() % 100 < 30)
          this->shoot();
      }
      else
        if (rand() % 100 < 50)
          setDirection(!Direction.x, !Direction.y);
        else
          this->shoot();
    }
    else
      if (rand() % 100 < 30)
        this->shoot();
  }
}

Bullet::Bullet(Tank* tank, UINT speed)
  :MovableObject(tank->getPosition().x, tank->getPosition().y, tank->getColor(), tank->getBackground(), 1, tank->getAttackDamage(), 0, 0)
{
  shooter_ = tank;
  setDirection(tank->getDirection());
  UINT TileSize = Game::instance().getWorld().getTileSize();
  speed_ = speed;
  setWidth(int(TileSize / 16));
  setHeight(TileSize / 3);
  //setOffset(TileSize / 2, TileSize / 2);
  setOffset(tank->getOffset().x + tank->getDirection().x*(TileSize / 3), tank->getOffset().y + tank->getDirection().y*(TileSize / 3));
  moveForward();
}

void Bullet::draw()
{
  VisualObject::draw();
}
void Bullet::drawTo(HDC hdc, HBITMAP hbitmap)
{
  if (isDead())
    return;
  if (VisualObject::beginDraw(hdc, hbitmap))
  {
    UINT TileSize = Game::instance().getWorld().getTileSize();
    POINT offset = getOffset();
    POINT Position = getScreenPositionCenter();

    int width = getWidth();
    if (width <= 0)
      width = 1;
    int height = getHeight();// int(TileSize / 3);
    POINT Direction = getDirection();

    if (getDirection().x == 0)
      Rectangle(hdc,
        Position.x - width, Position.y - height * ((getDirection().y < 0) ? 0 : -1),
        Position.x + width, Position.y + height * ((getDirection().y > 0) ? 0 : -1));
    else
      Rectangle(hdc,
        Position.x - height * ((getDirection().x < 0) ? 0 : -1), Position.y + width,
        Position.x + height * ((getDirection().x > 0) ? 0 : -1), Position.y - width);

    VisualObject::endDraw();
  };
}

void Bullet::update()
{
  int TileSize = Game::instance().getWorld().getTileSize();
  int MapSize = Game::instance().getWorld().getMapSize();
  if (isMooving())
  {
    POINT Direction = getDirection();
    POINT NewPosition = getPosition();
    NewPosition.x += (getOffset().x + (Direction.x * TileSize / 2)) / TileSize;
    NewPosition.y += (getOffset().y + (Direction.y * TileSize / 2)) / TileSize;
    setOffset(getOffset().x + Direction.x * speed_, getOffset().y + Direction.y * speed_);
    if (!Game::instance().getWorld().isValidPosition(NewPosition))
      stop();
    hitTest(NewPosition);
  }
}

void Bullet::update(double elapsed)
{
  int TileSize = Game::instance().getWorld().getTileSize();
  int MapSize = Game::instance().getWorld().getMapSize();
  if (isMooving())
  {
    POINT Direction = getDirection();
    POINT NewPosition = getPosition();
    NewPosition.x += (getOffset().x + (Direction.x * TileSize / 2)) / TileSize;
    NewPosition.y += (getOffset().y + (Direction.y * TileSize / 2)) / TileSize;
    double frameSpeed = elapsed * DEFAULT_BULLET_SPEED + modf(frameTime_, &frameTime_);
    frameTime_ = frameSpeed;
    setOffset(getOffset().x + lround(Direction.x * frameSpeed), getOffset().y + lround(Direction.y * frameSpeed));
    if (!Game::instance().getWorld().isValidPosition(NewPosition))
      stop();
    hitTest(NewPosition);
  }
}

bool Bullet::hitTest(POINT position)
{
  bool isFilledPosition = !Game::instance().getWorld().canMoveTo(position);
  if (!isFilledPosition)
    return false;
  GameObject* gameobject = Game::instance().getWorld().getObject(position);
  isFilledPosition = isFilledPosition && gameobject;
  bool selfShoot = gameobject == shooter_;
  bool toEnemy = shooter_->isEnemy(gameobject);
  bool toStatic = false;
  if (gameobject)
    toStatic = (!dynamic_cast<MovableObject*>(gameobject) && !gameobject->isWalkable());
  if (isFilledPosition && !selfShoot && (toEnemy || toStatic))
  {
    bool evade = !gameobject || !toStatic;
    if (gameobject && !toStatic)
    {
      //POINT Direction = getDirection();
      //POINT targetDirection = dynamic_cast<MovableObject*>(gameobject)->getDirection();
      //POINT targetOffset = dynamic_cast<MovableObject*>(gameobject)->getOffset();
      //try Evade (outway animation)
      //if (((targetDirection.x == Direction.x || targetDirection.y == Direction.y) && dynamic_cast<MovableObject*>(gameobject)->isMooving()))
      //  evade = ((abs(targetOffset.x) + abs(targetOffset.y)) < Game::instance().getTileSize() / 3);
      POINT targetCenter = dynamic_cast<MovableObject*>(gameobject)->getScreenPositionCenter();
      int myRadius = std::max(getHeight(), getWidth()) / 2;
      int targetRadius = std::max(dynamic_cast<MovableObject*>(gameobject)->getWidth(), dynamic_cast<MovableObject*>(gameobject)->getHeight()) / 2;
      evade = !circleIntersection(getScreenPositionCenter(), targetCenter, myRadius, targetRadius);//!toEnemy;
    }
    if (!evade)
    {
      gameobject->takeDamage(getAttackDamage(), getDirection());
      if (shooter_->isPlayer() && toEnemy)
        Game::instance().increaseScore();
      stop();
      GameObject::takeDamage(getHP());
      return true;
    }
  }
  return false;
}
