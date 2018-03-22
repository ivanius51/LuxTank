#pragma once
#include "stdafx.h"
#include "Command.h"
#include "GameObject.h"

void MoveForwardCommand::moveForwardCommand(GameObject* gameobject)
{
  object_ = gameobject;
}

void MoveForwardCommand::execute()
{
  if (dynamic_cast<MovableObject*>(object_))
    dynamic_cast<MovableObject*>(object_)->moveForward();
}

void TakeDamageCommand::takeDamageCommand(GameObject * gameobject, int damage)
{
  object_ = gameobject;
  damage_ = damage;
}

void TakeDamageCommand::execute()
{
  object_->takeDamage(damage_);
}

void FireCommand::fireCommand(GameObject* gameobject)
{
  object_ = gameobject;
}

void FireCommand::execute()
{
  if (dynamic_cast<Tank*>(object_))
    dynamic_cast<Tank*>(object_)->shoot();
}

void RotateCommand::rotateCommand(GameObject * gameobject, int x, int y)
{
  object_ = gameobject;
  direction_.x = x;
  direction_.y = y;
}

void RotateCommand::execute()
{
  if (dynamic_cast<MovableObject*>(object_))
    dynamic_cast<MovableObject*>(object_)->rotate(direction_);
}
