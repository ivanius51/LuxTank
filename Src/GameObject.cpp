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

Wall::Wall(int x, int y, UINT hp, int attackDamage, COLORREF color, COLORREF background, UINT width, UINT height)
  :VisualObject(x, y, hp, attackDamage, color, background, width, height)
{
  setIsWalkable(false);
}

Wall::Wall(POINT point, UINT hp, int attackDamage, COLORREF color, COLORREF background, UINT width, UINT height)
  : Wall(point.x, point.y, hp, attackDamage, color, background, width, height)
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

    Rectangle(hdc, getPosition().x*TileSize, getPosition().y*TileSize, (getPosition().x + 1)*TileSize, (getPosition().y + 1)*TileSize);
    if (getHP() < getMaxHP())
    {
      int stage = getMaxHP() / 4;
      int currentStage = getHP() / stage;
      //for (int i = getHP(); i < getMaxHP(); i += stage)
      {
        BitBlt(hdc, getPosition().x*TileSize, getPosition().y*TileSize, TileSize, (TileSize * 0.9 / currentStage), hdc, 0, 0, BLACKNESS);
      }
    }

    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    SelectObject(hdc, replaced);
  };
}

void Wall::update()
{
  if (isDead())
  {
    Game::instance().deleteObject(this);
    delete this;
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
  Wall::update();
  //gameover if dead
}

MovableObject::MovableObject(int x, int y, COLORREF color, COLORREF background, UINT hp, int attackDamage, UINT width, UINT height)
  :VisualObject(x, y, hp, attackDamage, color, background, width, height)
{
  isMooving_ = false;
}

void MovableObject::rotate(POINT point)
{
  if (!isMooving_)
    setDirection(point);
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

POINT VisualObject::getOffset()
{
  return offset_;
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

Tank::Tank(int x, int y, COLORREF color, COLORREF background, UINT hp, int attackDamage, UINT width, UINT height)
  :MovableObject(x, y, color, background, hp, attackDamage, width, height)
{
}

Tank::Tank(POINT point, COLORREF color, COLORREF background, UINT hp, int attackDamage, UINT width, UINT height)
  : Tank(point.x, point.y, color, background, hp, attackDamage, width, height)
{
}

void Tank::shoot()
{
  if (bullet && !bullet->isMooving())
  {
    delete bullet;
  }
  if (!bullet || !bullet->isMooving())
    //if ((GetTickCount() - ShootTime_)>ShootDelay)
    {
      //ShootTime_ = GetTickCount();
      bullet = new Bullet(this);
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

    HPEN pen = CreatePen(PS_SOLID, 1, getColor());
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    HBRUSH brush = CreateSolidBrush(getBackground());
    HGDIOBJ oldBrush = SelectObject(hdc, brush);

    UINT TileSize = Game::instance().getTileSize();
    POINT offset = getOffset();
    POINT screenPosition = getScreenPosition();//{ getPosition().x * TileSize + offset.x,getPosition().y * TileSize + offset.y };

    int width = int(TileSize / 5);
    int height = int(TileSize / 3);
    int center = round(TileSize / 2);

    Rectangle(hdc,
      screenPosition.x + center - width * ((getDirection().x < 0) ? 1 : 2), screenPosition.y + center - width * ((getDirection().y < 0) ? 1 : 2),
      screenPosition.x + center + width * ((getDirection().x > 0) ? 1 : 2), screenPosition.y + center + width * ((getDirection().y > 0) ? 1 : 2));

    width = 2;
    if (getDirection().x == 0)
      Rectangle(hdc,
        screenPosition.x + center - 1, screenPosition.y + center - height * ((getDirection().y < 0) ? 0 : -1),
        screenPosition.x + center + 1, screenPosition.y + center + height * ((getDirection().y > 0) ? 0 : -1));
    else
      Rectangle(hdc,
        screenPosition.x + center - height * ((getDirection().x < 0) ? 0 : -1), screenPosition.y + center + 1,
        screenPosition.x + center + height * ((getDirection().x > 0) ? 0 : -1), screenPosition.y + center - 1);

    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    SelectObject(hdc, replaced);
  };
  if (bullet)
    bullet->draw();
}

void Tank::update()
{
  // if turned reload image (use new turn)
  //if (getOldDirection().x != getDirection().x || getOldDirection().y != getDirection().y)
  //  ClearBuffer();

  int TileSize = Game::instance().getTileSize();
  int MapSize = Game::instance().getMapSize();

  POINT NewPosition = getPosition();
  NewPosition.x += getDirection().x;
  NewPosition.y += getDirection().y;

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
      setOffset(getOffset().x + getDirection().x, getOffset().y + getDirection().y);
      if (!Game::instance().canMoveTo(NewPosition) && (abs(getOffset().x) >= (TileSize / 5) || abs(getOffset().y) >= (TileSize / 5)))
      {
        stop();
        setOffset(0, 0);
      }
    }
  }
  //if AI - find player, gold and move to it or if see it - shoot

  if (bullet)
  {
    bullet->update();
    if (!bullet->isMooving())
    {
      delete bullet;
      bullet = nullptr;
    }
  }
}

Bullet::Bullet(Tank* tank)
  :MovableObject(tank->getPosition().x + tank->getDirection().x, tank->getPosition().y + tank->getDirection().y, tank->getColor(), tank->getBackground(), 0, tank->getAttackDamage(), 0, 0)
{
  UINT TileSize = Game::instance().getTileSize();
  setWidth(TileSize / 4);
  setHeight(TileSize / 4);
  setOffset(tank->getOffset().x - tank->getDirection().y*(TileSize / 2), tank->getOffset().y - tank->getDirection().x*(TileSize / 2));
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
    POINT screenPosition = getScreenPosition(); //{ getPosition().x * TileSize + offset.x,getPosition().y * TileSize + offset.y };

    int width = round(TileSize / 10) / 2;
    int height = round(TileSize / 3) / 2;
    int center = round(TileSize / 2);
    POINT Direction = getDirection();

    Rectangle(hdc,
      screenPosition.x - width - (Direction.x * height), screenPosition.y + center - width - (Direction.y * height),
      screenPosition.x + width + (Direction.x * height), screenPosition.y + center + width + (Direction.y * height));


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
      GameObject* gameobject = Game::instance().getObject(NewPosition);
      if (gameobject)
        gameobject->takeDamage(this->getAttackDamage());
    }
  }
}
