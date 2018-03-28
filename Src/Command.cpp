#include "stdafx.h"
#include "Command.h"
#include "GameObject.h"

Command::~Command()
{
}

MoveForwardCommand::MoveForwardCommand(GameObject* gameobject)
{
  object_ = gameobject;
}

void MoveForwardCommand::execute()
{
  if (dynamic_cast<MovableObject*>(object_))
    dynamic_cast<MovableObject*>(object_)->moveForward();
}

TakeDamageCommand::TakeDamageCommand(GameObject * gameobject, int damage)
{
  object_ = gameobject;
  damage_ = damage;
}

void TakeDamageCommand::execute()
{
  object_->takeDamage(damage_);
}

FireCommand::FireCommand(GameObject* gameobject)
{
  object_ = gameobject;
}

void FireCommand::execute()
{
  if (dynamic_cast<Tank*>(object_))
    dynamic_cast<Tank*>(object_)->shoot();
}

RotateCommand::RotateCommand(GameObject * gameobject, int x, int y)
{
  object_ = gameobject;
  direction_.x = x;
  direction_.y = y;
}

void RotateCommand::execute()
{
  if (dynamic_cast<MovableObject*>(object_))
  {
    if (object_->getDirection().x != direction_.x || object_->getDirection().y != direction_.y)
      dynamic_cast<MovableObject*>(object_)->rotate(direction_);
    else
      dynamic_cast<MovableObject*>(object_)->moveForward();
  }
}

StopForwardCommand::StopForwardCommand(GameObject * gameobject)
{
  object_ = gameobject;
}

void StopForwardCommand::execute()
{
  if (dynamic_cast<MovableObject*>(object_))
    dynamic_cast<MovableObject*>(object_)->stop();
}
