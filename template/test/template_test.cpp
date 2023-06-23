#include "template.h"

#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"


class templateTest : public ::testing::Test
{
protected:
    temp *myTemplate;
    virtual void SetUp() { myTemplate = new temp(); }

    virtual void TearDown() { delete myTemplate; }

};

TEST_F(templateTest, simpleAdd)
{
    int result = myTemplate->add(1,2);
    EXPECT_THAT(3, result);
}