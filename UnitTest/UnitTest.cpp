// UnitTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "gtest/gtest.h"
#include "GlobalVariables.h"
#include "GameObject.h"
#include "Game.h"

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

TEST(assertions, assertionstest)
{
  ASSERT_EQ(1, 0) << "1 is not equal 0";
}

TEST_F(MainTest, UnitTest1)
{
  
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  std::getchar();
  return result;
}