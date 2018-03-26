#ifndef WORLD_H
#define WORLD_H

#include <Windows.h>
#include <memory>

#include "GameObject.h"

class World
{
public:
  World();
  virtual ~World();
private:
  UINT tileSize_ = 0;
  UINT mapSize_ = 0;
  std::weak_ptr<GameObject> player_;
  std::weak_ptr<GameObject> gold_;
  std::vector<std::shared_ptr<GameObject>> tiles_;
  std::vector<std::shared_ptr<Bullet>> bullets_;
};

#endif // WORLD_H