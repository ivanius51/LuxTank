#include "stdafx.h"
#include "World.h"

#include <math.h>

#include "Game.h"
#include "GraphicEngine.h"

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

std::vector<std::shared_ptr<Bullet>>* World::getBullets()
{
  return &bullets_;
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

void World::objectsClear()
{
  tiles_.erase(
    std::remove_if(tiles_.begin(), tiles_.end(),
      [](std::shared_ptr<GameObject> object) {return object->isDead(); }
  ), tiles_.end());
}

void World::bulletsClear()
{
  bullets_.erase(
    std::remove_if(bullets_.begin(), bullets_.end(),
      [](std::shared_ptr<Bullet> bullet) {return (!bullet->isMooving() || bullet->isDead()); }
  ), bullets_.end());
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

bool World::isValidPosition(UINT x, UINT y)
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
  return (pointDistance(first, second) < VISIBLE_DISTANCE);
}

bool World::isIntersection(POINT first, POINT second)
{
  return ((first.x == second.x) || (first.y == second.y));
}

bool World::isNoEnemy()
{
  for (auto object = tiles_.begin(); object != tiles_.end(); object++)
    if (dynamic_cast<Tank*>(object->get()) && dynamic_cast<Tank*>(object->get())->isEnemy())
      return false;
  return true;
}

int World::enemyCount()
{
  int result = 0;
  for (auto object = tiles_.begin(); object != tiles_.end(); object++)
    if (dynamic_cast<Tank*>(object->get()) && dynamic_cast<Tank*>(object->get())->isEnemy())
      result++;
  return result;
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

bool World::canCreateTank(POINT position, int closeDistance)
{
  bool result = !getObject(position);
  if (result)
  {
    int xmin = std::max(0, int(position.x - closeDistance));
    int xmax = std::min(int(mapSize_ - 1), int(position.x + closeDistance));
    int ymin = std::max(0, int(position.y - closeDistance));
    int ymax = std::min(int(mapSize_ - 1), int(position.y + closeDistance));
    for (int x = xmin; x <= xmax; x++)
      for (int y = ymin; y <= ymax; y++)
      {
        POINT point = { x, y };
        GameObject* gameobject = getObject(point);
        if (gameobject && dynamic_cast<Tank*>(gameobject))
          result = false;
        if (!result)
          break;
      }
  }
  return result;
}

void World::generateNewMap()
{
  tiles_.erase(tiles_.begin(), tiles_.end());
  bullets_.erase(bullets_.begin(), bullets_.end());

  POINT point;
  std::shared_ptr<GameObject> gameobject;
  //random walls
  for (UINT i = 0; i < mapSize_; i++)
    for (UINT j = 0; j < mapSize_ - 2; j++)
      if (rand() % 100 < 20)
      {
        point = { lround(i), lround(j) };
        gameobject.reset(new Wall(point, WALL_HP, 0, WALL_TEXTURE, WALL_COLOR));
        //gameobject.get()->bindUpdateCallback([](GameObject& gameobject) {if (gameobject.isDead()) World::instance().stopGame(); });
        tiles_.push_back(std::shared_ptr<GameObject>(gameobject));
      }
  //Default player position
  point.x = mapSize_ / 2 + 2;
  point.y = mapSize_ - 1;
  gameobject.reset(new Tank(point, TANK_GREEN_1, ALLY_COLOR, PLAYER_LIVES));
  player_ = gameobject;
  dynamic_cast<Tank*>(player_.lock().get())->setPlayer(true);
  dynamic_cast<Tank*>(player_.lock().get())->setShootDelay(PLAYER_SHOOT_DELAY);
  tiles_.push_back(std::shared_ptr<GameObject>(gameobject));

  spawnEnemys();

  for (int i = int(mapSize_ / 2 - 1); i <= int(mapSize_ / 2 + 1); i++)
  {
    point = { i, int(mapSize_ - 2) };
    gameobject.reset(new Wall(point, WALL_HP, 0, WALL_TEXTURE, WALL_COLOR));
    tiles_.push_back(std::shared_ptr<GameObject>(gameobject));
    point = { i, int(mapSize_ - 1) };
    if (i == (mapSize_ / 2))
    {
      gameobject.reset(new Wall(point, GOLD_HP, 0, GOLD_TEXTURE, COLOR_YELLOW));
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

void World::spawnEnemys()
{
  POINT point;
  std::shared_ptr<GameObject> gameobject;
  int enemies = enemyCount();
  for (UINT i = 0; i <= UINT(mapSize_ * 0.7); i++)
    for (UINT j = 1; j < mapSize_; j++)
    {
      point = { lround(j), lround(i) };
      if ((rand() % 100 < 50) && (enemies < MAX_ENEMY_COUNT) &&
        (canCreateTank(point)))
      {
        gameobject.reset(new Tank(point, TANK_BLUE_1, ENEMY_COLOR, 1));
        dynamic_cast<Tank*>(gameobject.get())->setEnemy(true);
        if (rand() % 100 < 50)
          dynamic_cast<Tank*>(gameobject.get())->rotate({ 0, 1 });
        else
          if (rand() % 100 < 50)
            dynamic_cast<Tank*>(gameobject.get())->rotate({ 1, 0 });
          else
            if (rand() % 100 < 50)
              dynamic_cast<Tank*>(gameobject.get())->rotate({ -1, 0 });
        tiles_.push_back(std::shared_ptr<GameObject>(gameobject));
        enemies++;
      }
    }
}
