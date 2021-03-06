// UnitTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "gtest/gtest.h"
#include "GlobalVariables.cpp"
//#include "GameObject.h"
//#include "Game.h"

class MainTest : public ::testing::Test
{
public:
  MainTest()
  {
    // initialization code here
  }

  void SetUp()
  {
    // code here will execute just before the test ensues 
  }

  void TearDown()
  {
    // code here will be called just after the test completes
    // ok to through exceptions from here if need be
  }

  ~MainTest()
  {
    // cleanup any pending stuff, but no exceptions allowed
  }
};

#pragma region GlobalTests

TEST(GlobalTestFunctions, SQR)
{
  ASSERT_EQ(SQR(1), 1);
  ASSERT_EQ(SQR(-1), 1);
  ASSERT_EQ(SQR(0), 0);
  ASSERT_EQ(SQR(2), 4);
  ASSERT_EQ(SQR(3), 9);
}

TEST(GlobalTestFunctions, pointDistance)
{
  ASSERT_DOUBLE_EQ(pointDistance({ 0,0 }, { 0,0 }), 0.0);
  ASSERT_DOUBLE_EQ(pointDistance({ 10,-10 }, { 10,10 }), 20.0);
  ASSERT_NEAR(pointDistance({ 3,2 }, { 9,7 }), 7.81, 0.001);
}

TEST(GlobalTestFunctions, circleIntersection)
{
  ASSERT_EQ(circleIntersection({ 0,0 }, { 0,0 }, 10, 50), true);
  ASSERT_EQ(circleIntersection({ -10,-10 }, { 10,10 }, 10, 40), true);
  ASSERT_EQ(circleIntersection({ -243,42423 }, { 123,-4234 }, 0, 0), false);
  ASSERT_EQ(circleIntersection({ -243,42423 }, { 123,-4234 }, 1000, 1000), false);
}

TEST(GlobalTestFunctions, sign)
{
  ASSERT_EQ(sign(-23.534534), -1);
  ASSERT_EQ(sign(54353.354345), 1);
  ASSERT_EQ(sign(234), 1);
  ASSERT_EQ(sign(-54234), -1);
  ASSERT_EQ(sign(-1), -1);
  ASSERT_EQ(sign(1), 1);
  ASSERT_EQ(sign(0), 0);
  ASSERT_EQ(sign(-1.0), -1);
  ASSERT_EQ(sign(1.0), 1);
  ASSERT_EQ(sign(0.0), 0);
  ASSERT_EQ(sign(""), 0) << "no logic here, must be compile\run error, but have some value";
}

TEST(GlobalTestValues, TILE_SIZE)
{
  ASSERT_EQ((TILE_SIZE & (TILE_SIZE - 1)), 0);
  ASSERT_GE(TILE_SIZE, 8) << ">=8";
  ASSERT_LE(TILE_SIZE, 256) << "<=256";
}

TEST(GlobalTestValues, MAP_SIZE)
{
  ASSERT_NE(MAP_SIZE % 2, 0);
  ASSERT_GE(MAP_SIZE, 1) << ">=1";
  ASSERT_LE(MAP_SIZE, 1024) << "<=1024";
  ASSERT_LE(VISIBLE_DISTANCE, MAP_SIZE) << "<=MAP_SIZE";
}

TEST(GlobalTestValues, MAX_FPS)
{
  ASSERT_GE(MAX_FPS, 15) << ">=15";
}

TEST(GlobalTestValues, Lives)
{
  ASSERT_GE(GOLD_HP, 1) << ">=1";
  ASSERT_GE(WALL_HP, 1) << ">=1";
  ASSERT_GE(PLAYER_LIVES, 1) << ">=1";
}

TEST(GlobalTestValues, Speed)
{
  ASSERT_GT(DEFAULT_BULLET_SPEED, 0) << ">0";
  ASSERT_LT(DEFAULT_BULLET_SPEED, 1) << "<1";
  ASSERT_GT(DEFAULT_OBJECT_SPEED, 0) << ">0";
  ASSERT_LT(DEFAULT_OBJECT_SPEED, 1) << "<1";
}

TEST(GDIFunctions, Creation)
{
  HBRUSH brush = nullptr;
  ASSERT_EQ(brush, nullptr);
  brush = gdi::createBrush(BS_SOLID, 0, NULL);
  ASSERT_NE(brush, nullptr);
  DeleteObject(brush);
  ASSERT_NE(brush, nullptr);
  HPEN pen = nullptr;
  ASSERT_EQ(pen, nullptr);
  pen = gdi::createPen(PS_SOLID, 0, 0);
  ASSERT_NE(pen, nullptr);
  DeleteObject(pen);
  ASSERT_NE(pen, nullptr);
}
#pragma endregion

TEST_F(MainTest, UnitTest1)
{

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  std::getchar();
  return result;
}