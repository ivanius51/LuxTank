#pragma once
#ifndef COMMAND_H
#define COMMAND_H

#include "GameObject.h"

class Command
{
public:
  virtual void execute() = 0;
};

class MoveForwardCommand : Command
{
  private:
    GameObject* object_;
  public:
    void moveForwardCommand(GameObject* gameobject);
    void execute();
};

class TakeDamageCommand : Command
{
  private:
    GameObject* object_;
    int damage_ = 1;
  public:
    void takeDamageCommand(GameObject* gameobject, int damage = 1);
    void execute();
};

class RotateCommand : Command
{
  private:
    GameObject* object_;
    POINT direction_;
  public:
    void rotateCommand(GameObject* gameobject, int x, int y);
    void execute();
};

class FireCommand : Command
{
  private:
    GameObject* object_;
  public:
    void fireCommand(GameObject* gameobject);
    void execute();
};

#endif // COMMAND_H