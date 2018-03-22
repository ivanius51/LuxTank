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

UINT GameObject::getHP()
{
  return hp_;
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
  hp_ += damage;
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

void GameObject::
setDirection(int x, int y)
{
  direction_.x = x;
  direction_.y = y;
}

void GameObject::setDirection(POINT point)
{
  direction_ = point;
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
  //targetdc_ = Game::instance().getHdc();
  //targeBitmap_ = Game::instance().getHdc();
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

}

Wall::Wall(POINT point, UINT hp, int attackDamage, COLORREF color, COLORREF background, UINT width, UINT height)
  : Wall(point.x, point.y, hp, attackDamage, color, background, width, height)
{
}

void Wall::draw()
{
  if (targetdc_ && targeBitmap_)
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

    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    SelectObject(hdc, replaced);
  };
}

void Wall::update()
{
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
}

void Gold::update()
{
  Wall::update();
  //gameover if dead
}

MovableObject::MovableObject(int x, int y, COLORREF color, COLORREF background, UINT hp, int attackDamage, UINT width, UINT height)
  :VisualObject(x, y, hp, attackDamage, color, background, width, height)
{

}

void MovableObject::rotate(POINT point)
{
}

void MovableObject::moveForward()
{
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
}

void Tank::draw()
{
  if (targetdc_ && targeBitmap_)
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
    POINT screenPosition = { getPosition().x * TileSize + offset.x,getPosition().y * TileSize + offset.y };

    //TODO: replace magic numbers to calculation from tilesize
    int width = int(TileSize / 5);
    int height = int(TileSize / 3);
    int center = round(TileSize / 2);

    Rectangle(hdc,
      screenPosition.x + center - width * ((getDirection().x < 0) ? 1 : 2), screenPosition.y + center - width * ((getDirection().y < 0) ? 1 : 2),
      screenPosition.x + center + width * ((getDirection().x > 0) ? 1 : 2), screenPosition.y + center + width * ((getDirection().y > 0) ? 1 : 2));

    width = 2;
    if (getDirection().x == 0)
      Rectangle(hdc,
        screenPosition.x + 14, screenPosition.y + center - height * ((getDirection().y < 0) ? 0 : -1),
        screenPosition.x + 17, screenPosition.y + center + height * ((getDirection().y > 0) ? 0 : -1));
    else
      Rectangle(hdc,
        screenPosition.x + center - height * ((getDirection().x < 0) ? 0 : -1), screenPosition.y + 14,
        screenPosition.x + center + height * ((getDirection().x > 0) ? 0 : -1), screenPosition.y + 17);

    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    SelectObject(hdc, replaced);
  };
}

void Tank::update()
{
  //if AI - find player, gold and move to it or if see it - shoot
}

Bullet::Bullet(Tank* tank)
  :MovableObject(tank->getPosition().x + tank->getDirection().x, tank->getPosition().y + tank->getDirection().y, tank->getColor(), tank->getBackground(), 0, tank->getAttackDamage(), 0, 0)
{

}

void Bullet::draw()
{
  if (targetdc_ && targeBitmap_)
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
    POINT screenPosition = { getPosition().x * TileSize + offset.x,getPosition().y * TileSize + offset.y };

    int width = round(TileSize / 10) / 2;
    int height = round(TileSize / 3) / 2;
    int center = round(TileSize / 2);
    POINT Direction = getDirection();

    Rectangle(hdc,
      center - width - (Direction.x * height), center - width - (Direction.y * height),
      center + width + (Direction.x * height), center + width + (Direction.y * height));


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
    if (Game::instance().isValidPosition(NewPosition))
    {

    }
  }
}
