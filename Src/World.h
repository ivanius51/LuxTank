#ifndef WORLD_H
#define WORLD_H

#include "GameObject.h"

class World
{
public:
  World(UINT mapSize = MAP_SIZE, UINT tileSize = TILE_SIZE);
  virtual ~World();
  //canCreateobject can remake to object type as param
  bool canCreateTank(POINT position, int closeDistance = 2);
  void generateNewMap();
  void spawnEnemys();

  GameObject* getPlayer();
  GameObject* getGold();
  bool isNoEnemy();
  int enemyCount();
  std::vector<std::shared_ptr<GameObject>>* getTiles();
  std::vector<std::shared_ptr<Bullet>>* getBullets();

  void objectsClear();
  void bulletsClear();

  GameObject* getObject(int x, int y);
  GameObject* getObject(POINT point);
  bool deleteObject(GameObject* gameobject);
  bool isWalkable(int x, int y);
  bool isWalkable(POINT point);
  bool isValidPosition(POINT point);
  bool isValidPosition(UINT x, UINT y);
  bool canMoveTo(POINT point);
  bool canMoveTo(int x, int y);
  bool isInVisibleDistance(POINT first, POINT second);
  bool isIntersection(POINT first, POINT second);
  //bool checkCollision(int x, int y);
  //bool checkCollision(POINT point);
  GameObject* collidedWith(GameObject* gameobject);
  UINT getTileSize();
  UINT getMapSize();

private:
  UINT tileSize_ = 0;
  UINT mapSize_ = 0;
  std::weak_ptr<GameObject> player_;
  std::weak_ptr<GameObject> gold_;
  std::vector<std::shared_ptr<GameObject>> tiles_;
  std::vector<std::shared_ptr<Bullet>> bullets_;
};

#endif // WORLD_H