//#pragma once
#ifndef COMMAND_H
#define COMMAND_H

#include "GameObject.h"

class Command
{
public:
  virtual void execute() = 0;
};

class MoveForwardCommand : public Command
{
  private:
    GameObject* object_;
  public:
    MoveForwardCommand(GameObject* gameobject);
    void execute();
};

class TakeDamageCommand : public Command
{
  private:
    GameObject* object_;
    int damage_ = 1;
  public:
    TakeDamageCommand(GameObject* gameobject, int damage = 1);
    void execute();
};

class RotateCommand : public Command
{
  private:
    GameObject* object_;
    POINT direction_;
  public:
    RotateCommand(GameObject* gameobject, int x, int y);
    void execute();
};

class FireCommand : public Command
{
  private:
    GameObject* object_;
  public:
    FireCommand(GameObject* gameobject);
    void execute();
};

#endif // COMMAND_H