#include "stdafx.h"
#include "World.h"

#include <algorithm>
#include <math.h>

#include "Game.h"


World::World(UINT mapSize, UINT tileSize)
{
  mapSize_ = mapSize;
  tileSize_ = tileSize;
}

World::~World()
{
}

GameObject* World::getPlayer()
{
  return player_.lock().get();
}

GameObject* World::getGold()
{
  return gold_.lock().get();
}

std::vector<std::shared_ptr<GameObject>>* World::getTiles()
{
  return &tiles_;
}

GameObject* World::getObject(POINT point)
{
  auto tile = std::find_if(tiles_.begin(), tiles_.end(),
    [&point](std::shared_ptr<GameObject> object)
  {return object->getPosition().x == point.x && object->getPosition().y == point.y; }
  );
  if (tile != tiles_.end())
    return tile->get();
  else
    return nullptr;
}

bool World::deleteObject(GameObject* gameobject)
{
  auto tile = std::find_if(tiles_.begin(), tiles_.end(),
    [&gameobject](std::shared_ptr<GameObject> object)
  {return object.get() == gameobject; }
  );
  if (tile != tiles_.end())
  { 
    tiles_.erase(tile);
    return true;
  }
  return false;
}

GameObject* World::getObject(int x, int y)
{
  POINT point;
  point.x = x;
  point.y = y;
  return World::getObject(point);
}

bool World::isWalkable(int x, int y)
{
  GameObject* gameobject = World::getObject(x, y);
  return ((!gameobject) || gameobject->isWalkable());
}

bool World::isWalkable(POINT point)
{
  return isWalkable(point.x, point.y);
}

bool World::isValidPosition(POINT point)
{
  return isValidPosition(point.x, point.y);
}

bool World::isValidPosition(int x, int y)
{
  return ((x >= 0) && (x < mapSize_)) && ((y >= 0) && (y < mapSize_));
}

bool World::canMoveTo(POINT point)
{
  return canMoveTo(point.x, point.y);
}

bool World::canMoveTo(int x, int y)
{
  return (isValidPosition(x, y) && isWalkable(x, y));
}

bool World::isInVisibleDistance(POINT first, POINT second)
{
  return (sqrt(SQR(second.x - first.x) + SQR(second.y - first.y)) < VISIBLE_DISTANCE);
}

bool World::isIntersection(POINT first, POINT second)
{
  return ((first.x == second.x) || (first.y == second.y));
}

GameObject* World::collidedWith(GameObject* gameobject)
{
  if (!dynamic_cast<VisualObject*>(gameobject))
    return nullptr;

  POINT Direction = gameobject->getDirection();
  POINT Position = gameobject->getPosition();
  POINT Offset = dynamic_cast<VisualObject*>(gameobject)->getOffset();
  Position.x += (Offset.x + (Direction.x * tileSize_ / 2)) / 2;
  Position.y += (Offset.y + (Direction.y * tileSize_ / 2)) / 2;
  GameObject* interact = getObject(Position);
  if (!interact || !dynamic_cast<VisualObject*>(interact))
    return nullptr;

  POINT ScreenPosition1 = dynamic_cast<VisualObject*>(gameobject)->getScreenPosition();
  int Radius1 = std::max(dynamic_cast<VisualObject*>(gameobject)->getWidth(), dynamic_cast<VisualObject*>(gameobject)->getHeight()) / 2;
  POINT ScreenPosition2 = dynamic_cast<VisualObject*>(interact)->getScreenPosition();
  int Radius2 = std::max(dynamic_cast<VisualObject*>(interact)->getWidth(), dynamic_cast<VisualObject*>(interact)->getHeight()) / 2;

  int x = ScreenPosition1.x - ScreenPosition2.x;
  int y = ScreenPosition1.y - ScreenPosition2.y;

  int distance = x * x + y * y;
  int maxdistance = (Radius2 + Radius1) * (Radius2 + Radius1);

  if (distance <= maxdistance)
    return interact;
  else
    return nullptr;
}

UINT World::getTileSize()
{
  return tileSize_;
}

UINT World::getMapSize()
{
  return mapSize_;
}

void World::generateNewMap()
{
  POINT point;
  std::shared_ptr<GameObject> gameobject;
  //random walls
  for (int i = 0; i < mapSize_; i++)
    for (int j = 0; j < mapSize_ - 2; j++)
      if (rand() % 100 < 20)
      {
        point = { i, j };
        gameobject.reset(new Wall(point, WALL_HP, 0, WALL_TEXTURE));
        //gameobject.get()->bindUpdateCallback([](GameObject& gameobject) {if (gameobject.isDead()) World::instance().stopGame(); });
        tiles_.push_back(std::shared_ptr<GameObject>(gameobject));
      }
  //Default player position
  point.x = mapSize_ / 2 + 2;
  point.y = mapSize_ - 1;
  gameobject.reset(new Tank(point, TANK_GREEN_1, ALLY_COLOR, PLAYER_LIVES));
  player_ = gameobject;
  dynamic_cast<Tank*>(player_.lock().get())->setPlayer(true);
  tiles_.push_back(std::shared_ptr<GameObject>(gameobject));

  for (int i = 0; i <= int(mapSize_ * 0.75); i++)
    for (int j = 0; j < mapSize_; j++)
    {
      point = { j, i };
      if ((i % 3 == 0) && (j % 3 == 0) && (rand() % 100 < 25) && (!getObject(point)))
      {
        gameobject.reset(new Tank(point, TANK_BLUE_1, ENEMY_COLOR, 1));
        dynamic_cast<Tank*>(gameobject.get())->setEnemy(true);
        tiles_.push_back(std::shared_ptr<GameObject>(gameobject));
      }
    }

  for (int i = int(mapSize_ / 2 - 1); i <= int(mapSize_ / 2 + 1); i++)
  {
    point = { i, int(mapSize_ - 2) };
    gameobject.reset(new Wall(point, WALL_HP, 0, WALL_TEXTURE));
    tiles_.push_back(std::shared_ptr<GameObject>(gameobject));
    point = { i, int(mapSize_ - 1) };
    if (i == (mapSize_ / 2))
    {
      gameobject.reset(new Wall(point, GOLD_HP, 0, COLOR_YELLOW / 2, COLOR_YELLOW));
      gold_ = gameobject;
      gameobject.get()->bindUpdateCallback([](GameObject& gameobject) {if (gameobject.isDead()) Game::instance().stopGame(); });
      tiles_.push_back(std::shared_ptr<GameObject>(gameobject));
    }
    else
    {
      gameobject.reset(new Wall(point, WALL_HP, 0, WALL_TEXTURE));
      tiles_.push_back(std::shared_ptr<GameObject>(gameobject));
    }
  }
}