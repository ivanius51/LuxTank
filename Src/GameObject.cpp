#include "stdafx.h"

#include <algorithm>

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
    width_ = Game::instance().getTileSize();
  if (height)
    height_ = height;
  else
    height_ = Game::instance().getTileSize();
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
  UINT TileSize = Game::instance().getTileSize();
  return POINT({ int(getPosition().x * TileSize + getOffset().x), int(getPosition().y * TileSize + getOffset().y) });
}

Wall::Wall(int x, int y, UINT hp, int attackDamage, COLORREF color, COLORREF background, UINT width, UINT height)
  :VisualObject(x, y, hp, attackDamage, color, background, "", width, height)
{
  setIsWalkable(false);
}

Wall::Wall(POINT point, UINT hp, int attackDamage, COLORREF color, COLORREF background, UINT width, UINT height)
  : Wall(point.x, point.y, hp, attackDamage, color, background, width, height)
{
}

Wall::Wall(int x, int y, UINT hp, int attackDamage, const std::string & texture, UINT width, UINT height)
  : VisualObject(x, y, hp, attackDamage, COLOR_GREY, COLOR_GREY, texture, width, height)
{
  setIsWalkable(false);
}

Wall::Wall(POINT point, UINT hp, int attackDamage, const std::string & texture, UINT width, UINT height)
  : Wall(point.x, point.y, hp, attackDamage, texture, width, height)
{
}

void Wall::draw()
{
  if (targetdc_ != 0 && targeBitmap_ != 0)
    drawTo(targetdc_, targeBitmap_);
  else
    drawTo(Game::instance().getBufferDc(), Game::instance().getBuffer());
}

void Wall::drawTo(HDC hdc, HBITMAP hbitmap)
{
  if (isDead())
    return;
  HGDIOBJ replaced = SelectObject(hdc, hbitmap);
  if (replaced)
  {
    targetdc_ = hdc;
    targeBitmap_ = hbitmap;

    UINT TileSize = Game::instance().getTileSize();
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
      HPEN pen = CreatePen(PS_SOLID, 1, getColor());
      HGDIOBJ oldPen = SelectObject(hdc, pen);
      HBRUSH brush = CreateSolidBrush(COLOR_BLACK);
      HGDIOBJ oldBrush = SelectObject(hdc, brush);

      Rectangle(hdc,
        Position.x, Position.y,
        Position.x + TileSize, Position.y + TileSize);

      SelectObject(hdc, GetStockObject(DC_BRUSH));
      SetDCBrushColor(hdc, getBackground());

      Rectangle(hdc,
        Position.x + Destrucions.left * stage, Position.y + Destrucions.top * stage,
        Position.x + TileSize - Destrucions.right * stage, Position.y + TileSize - Destrucions.bottom * stage);

      SelectObject(hdc, oldBrush);
      DeleteObject(brush);
      SelectObject(hdc, oldPen);
      DeleteObject(pen);
    }
    SelectObject(hdc, replaced);
  };
}

void Wall::update()
{
  if (isDead())
  {
    Game::instance().deleteObject(this);
    //delete this;
  }
}

Gold::Gold(UINT x, UINT y) :
  Wall(x, y, 1, 0, RGB(128, 128, 0), RGB(255, 255, 0))
{
}

Gold::Gold(POINT point)
  : Gold(point.x, point.y)
{
}

void Gold::draw()
{
  Wall::draw();
}

void Gold::drawTo(HDC hdc, HBITMAP hbitmap)
{
  Wall::drawTo(hdc, hbitmap);
}

void Gold::update()
{
  if (isDead())
    Game::instance().stopGame();
}

MovableObject::MovableObject(int x, int y, COLORREF color, COLORREF background, UINT hp, int attackDamage, UINT width, UINT height)
  :VisualObject(x, y, hp, attackDamage, color, background, width, height)
{
  isMooving_ = false;
}

MovableObject::MovableObject(int x, int y, UINT hp, int attackDamage, const std::string & texture, COLORREF color, COLORREF background, UINT width, UINT height)
  : VisualObject(x, y, hp, attackDamage, color, background, texture, width, height)
{
  isMooving_ = false;
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
  return ((GetTickCount() - updateTime_) - updateDelay_);
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
}

Tank::Tank(POINT point, COLORREF color, COLORREF background, UINT hp, int attackDamage, UINT width, UINT height)
  : Tank(point.x, point.y, color, background, hp, attackDamage, width, height)
{
}

Tank::Tank(int x, int y, const std::string & texture, COLORREF color, UINT hp, int attackDamage, UINT width, UINT height)
  : MovableObject(x, y, hp, attackDamage, texture, color, color, width, height)
{
}

Tank::Tank(POINT point, const std::string & texture, COLORREF color, UINT hp, int attackDamage, UINT width, UINT height)
  : Tank(point.x, point.y, texture, color, hp, attackDamage, width, height)
{
}

bool Tank::isEnemy()
{
  return isenemy_;
}

bool Tank::isPlayer()
{
  return isplayer_;
}

void Tank::shoot()
{
  //if (bullet_ && !bullet_->isMooving())
  //{
  //  delete bullet_;
  //}
  //if (!bullet_ || !bullet_->isMooving())
  if ((GetTickCount() - ShootTime_) > SHOOT_DELAY)
  {
    ShootTime_ = GetTickCount();
    Game::instance().addBullet(std::make_shared<Bullet>(this));
    //bullet_ = new Bullet(this);
  }
}

void Tank::draw()
{
  if (targetdc_ != 0 && targeBitmap_ != 0)
    drawTo(targetdc_, targeBitmap_);
  else
    drawTo(Game::instance().getBufferDc(), Game::instance().getBuffer());
}

void Tank::drawTo(HDC hdc, HBITMAP hbitmap)
{
  HGDIOBJ replaced = SelectObject(hdc, hbitmap);
  if (replaced)
  {
    targetdc_ = hdc;
    targeBitmap_ = hbitmap;

    UINT TileSize = Game::instance().getTileSize();
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
      gdi::rotateTexture(texture, degres, true);
      gdi::drawBitmap(hdc, Position.x, Position.y, TileSize, TileSize, texture, true);
      DeleteObject(texture);
    }
    else
    {
      HPEN pen = CreatePen(PS_SOLID, 1, getColor());
      HGDIOBJ oldPen = SelectObject(hdc, pen);
      HBRUSH brush = CreateSolidBrush(getBackground());
      HGDIOBJ oldBrush = SelectObject(hdc, brush);

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

      SelectObject(hdc, oldBrush);
      DeleteObject(brush);
      SelectObject(hdc, oldPen);
      DeleteObject(pen);
    }
    SelectObject(hdc, replaced);
  };
  //if (bullet_)
  //  bullet_->draw();
}

void Tank::update()
{
  int TileSize = Game::instance().getTileSize();
  int MapSize = Game::instance().getMapSize();

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
      if (Game::instance().canMoveTo(NewPosition))
        setPosition(NewPosition);
    }
    else
    {
      setOffset(getOffset().x + Direction.x, getOffset().y + Direction.y);
      if (!Game::instance().canMoveTo(NewPosition) && (abs(getOffset().x) >= (TileSize / 5) || abs(getOffset().y) >= (TileSize / 5)))
      {
        stop();
        setOffset(0, 0);
      }
    }
  }

  //TODO: Enemy Actions move it to some AI class.method
  if (isEnemy() && canUpdate())
  {
    updateTime_ = GetTickCount();
    //if AI - find player, gold and move to it or if see it - shoot
    GameObject* Player = Game::instance().getPlayer();
    //see player
    if (Game::instance().isInVisibleDistance(getPosition(), Player->getPosition()))
    {
      //can turn and shoot
      if (Game::instance().isIntersection(getPosition(), Player->getPosition()))
      {
        rotateTo(Player->getPosition());
        this->shoot();
      }
      else
        if (!isMooving())
        {
          if (Game::instance().canMoveTo(NewPosition))
            moveForward();
          else
          {
            setDirection(!Direction.x, !Direction.y);
          }
        }
    }
  }

  //check self bullets
  //bullets_.erase(
  //  std::remove_if(bullets_.begin(), bullets_.end(), 
  //    [](MovableObject* movableobject){return movableobject->isMooving();}
  //  ), bullets_.end());
  //take indexes and remove after
  //(std::vector<Bullet*>::const_iterator it;
  //for(auto it = bullets_.begin(); it != bullets_.end(); it++)
  //{
  //  if (*it)
  //    (*it)->update();
  //}

  //if (bullet_)
  //{
  //  bullet_->update();
  //  if (!bullet_->isMooving())
  //  {
  //    delete bullet_;
  //    bullet_ = nullptr;
  //  }
  //}
}

Bullet::Bullet(Tank* tank)
  :MovableObject(tank->getPosition().x, tank->getPosition().y, tank->getColor(), tank->getBackground(), 0, tank->getAttackDamage(), 0, 0)
{
  setDirection(tank->getDirection());
  UINT TileSize = Game::instance().getTileSize();
  setWidth(TileSize / 4);
  setHeight(TileSize / 4);
  setOffset(tank->getOffset().x + tank->getDirection().x*(TileSize / 3), tank->getOffset().y + tank->getDirection().y*(TileSize / 3));
  moveForward();
}

void Bullet::draw()
{
  if (targetdc_ != 0 && targeBitmap_ != 0)
    drawTo(targetdc_, targeBitmap_);
  else
    drawTo(Game::instance().getBufferDc(), Game::instance().getBuffer());
}
void Bullet::drawTo(HDC hdc, HBITMAP hbitmap)
{
  HGDIOBJ replaced = SelectObject(hdc, hbitmap);
  if (replaced)
  {
    targetdc_ = hdc;
    targeBitmap_ = hbitmap;

    HPEN pen = CreatePen(PS_SOLID, 1, getColor());
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    HBRUSH brush = CreateSolidBrush(getBackground());
    HGDIOBJ oldBrush = SelectObject(hdc, brush);

    UINT TileSize = Game::instance().getTileSize();
    POINT offset = getOffset();
    POINT ScreenPosition = getScreenPosition();//{ getPosition().x * TileSize + offset.x,getPosition().y * TileSize + offset.y };

    int width = int(TileSize / 25);
    if (width <= 0)
      width = 1;
    int height = int(TileSize / 4);
    int center = int(TileSize / 2);
    POINT Direction = getDirection();

    if (getDirection().x == 0)
      Rectangle(hdc,
        ScreenPosition.x + center - width, ScreenPosition.y + center - height * ((getDirection().y < 0) ? 0 : -1),
        ScreenPosition.x + center + width, ScreenPosition.y + center + height * ((getDirection().y > 0) ? 0 : -1));
    else
      Rectangle(hdc,
        ScreenPosition.x + center - height * ((getDirection().x < 0) ? 0 : -1), ScreenPosition.y + center + width,
        ScreenPosition.x + center + height * ((getDirection().x > 0) ? 0 : -1), ScreenPosition.y + center - width);

    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    SelectObject(hdc, replaced);
  };
}

void Bullet::update()
{
  int TileSize = Game::instance().getTileSize();
  int MapSize = Game::instance().getMapSize();
  if (isMooving())
  {
    POINT Direction = getDirection();
    POINT NewPosition = getPosition();
    NewPosition.x += (getOffset().x + (Direction.x * TileSize / 2)) / TileSize;
    NewPosition.y += (getOffset().y + (Direction.y * TileSize / 2)) / TileSize;
    if (Game::instance().canMoveTo(NewPosition))
    {
      setOffset(getOffset().x + getDirection().x, getOffset().y + getDirection().y);
    }
    else
    {
      stop();
      GameObject* gameobject = Game::instance().getObject(NewPosition).get();
      if (gameobject)
        gameobject->takeDamage(getAttackDamage(), getDirection());
    }
  }
}
