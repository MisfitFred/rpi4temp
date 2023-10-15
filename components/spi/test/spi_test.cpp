#include "spi.h"

#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"



class templateTest : public ::testing::Test
{
protected:
    spi *mySpi;
    virtual void SetUp() { mySpi = new spi(); }

    virtual void TearDown() { delete mySpi; }
};

TEST_F(templateTest, simpleAdd)
{
    int result = 3;
    EXPECT_THAT(3, result);
}