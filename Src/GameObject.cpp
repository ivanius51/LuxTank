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
  HGDIOBJ replaced = SelectObject(Game::instance().getBufferDc(), Game::instance().getBuffer());
  if (replaced)
  {
    HPEN pen = CreatePen(PS_SOLID, 1, getColor());
    HGDIOBJ oldPen = SelectObject(Game::instance().getBufferDc(), pen);
    HBRUSH brush = CreateSolidBrush(getBackground());
    HGDIOBJ oldBrush = SelectObject(Game::instance().getBufferDc(), brush);

    Rectangle(Game::instance().getBufferDc(), getPosition().x*Game::instance().getTileSize(), getPosition().y*Game::instance().getTileSize(), (getPosition().x + 1)*Game::instance().getTileSize(), (getPosition().y + 1)*Game::instance().getTileSize());

    SelectObject(Game::instance().getBufferDc(), oldBrush);
    DeleteObject(brush);
    SelectObject(Game::instance().getBufferDc(), oldPen);
    DeleteObject(pen);

    SelectObject(Game::instance().getBufferDc(), replaced);
  };
}

void Wall::drawTo(HDC hdc, HBITMAP hbitmap)
{
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
  drawTo(Game::instance().getBufferDc(), Game::instance().getBuffer());
}

void Tank::drawTo(HDC hdc, HBITMAP hbitmap)
{
  HGDIOBJ replaced = SelectObject(hdc, hbitmap);
  if (replaced)
  {
    HPEN pen = CreatePen(PS_SOLID, 1, getColor());
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    HBRUSH brush = CreateSolidBrush(getBackground());
    HGDIOBJ oldBrush = SelectObject(hdc, brush);

    UINT tileSize = Game::instance().getTileSize();
    POINT screenPosition = { getPosition().x*tileSize,getPosition().y*tileSize };

    int width = 6;
    int height = 12;
    Rectangle(hdc, screenPosition.x + 16 - width*((getDirection().x < 0) ? 1 : 2), screenPosition.y + 16 - width*((getDirection().y < 0) ? 1 : 2), screenPosition.x + 16 + width*((getDirection().x > 0) ? 1 : 2), screenPosition.y + 16 + width*((getDirection().y > 0) ? 1 : 2));

    width = 2;
    if (getDirection().x == 0)
      Rectangle(hdc, screenPosition.x + 14, screenPosition.y + 16 - height*((getDirection().y < 0) ? 0 : -1), screenPosition.x + 17, screenPosition.y + 16 + height*((getDirection().y > 0) ? 0 : -1));
    else
      Rectangle(hdc, screenPosition.x + 16 - height*((getDirection().x < 0) ? 0 : -1), screenPosition.y + 14, screenPosition.x + 16 + height*((getDirection().x > 0) ? 0 : -1), screenPosition.y + 17);

    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    SelectObject(hdc, replaced);
  };

  
}

void Tank::update()
{
}

Bullet::Bullet(Tank* tank)
  :MovableObject(tank->getPosition().x + tank->getDirection().x, tank->getPosition().y + tank->getDirection().y, tank->getColor(), tank->getBackground(), 0, tank->getAttackDamage(), 0, 0)
{
}

void Bullet::draw()
{
}

void Bullet::update()
{
}
