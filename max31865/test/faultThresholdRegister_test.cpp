

#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define UNIT_TEST
#include "max31865.h"


class faultThresholdRegisterTest : public ::testing::Test
{
protected:
    faultThresholdRegister_t *myfaultThresholdRegister;
    virtual void SetUp() { myfaultThresholdRegister = new faultThresholdRegister_t(); }

    virtual void TearDown() { delete myfaultThresholdRegister; }

};

TEST_F(faultThresholdRegisterTest, get_set_Threshold)
{
    uint16_t actualThreshold = myfaultThresholdRegister->getThreshold();
    EXPECT_EQ(0, actualThreshold);

    uint16_t expectedThreshold = 0x1234;
    myfaultThresholdRegister->setThreshold(expectedThreshold);
    actualThreshold = myfaultThresholdRegister->getThreshold();
    EXPECT_EQ(expectedThreshold, actualThreshold);
}

TEST_F(faultThresholdRegisterTest, print)
{
    uint16_t expectedThreshold = 0x1234;
    myfaultThresholdRegister->setThreshold(expectedThreshold);
    testing::internal::CaptureStdout();
    myfaultThresholdRegister->print();
    std::string actualPrint = testing::internal::GetCapturedStdout();
    std::string expectedPrint =
        "threshold: 4660\n";
    EXPECT_EQ(expectedPrint, actualPrint);
}

TEST_F(faultThresholdRegisterTest, toString)
{
    uint16_t expectedThreshold = 0x1234;
    myfaultThresholdRegister->setThreshold(expectedThreshold);
    std::string actualPrint = myfaultThresholdRegister->toString();
    std::string expectedPrint =
        "threshold: 4660\n";
    EXPECT_EQ(expectedPrint, actualPrint);
}

TEST_F(faultThresholdRegisterTest, update_getThreshold)
{
    uint16_t actualThreshold = myfaultThresholdRegister->getThreshold();
    EXPECT_EQ(0, actualThreshold);

    uint8_t registerRawValue_uint8p[2] = {0x12,0x32}; //0x1232
    uint16_t expectedThreshold = 0x919;
    myfaultThresholdRegister->updateRawValue(registerRawValue_uint8p);
    actualThreshold = myfaultThresholdRegister->getThreshold();
    EXPECT_EQ(expectedThreshold, actualThreshold);
}
TEST_F(faultThresholdRegisterTest, getThresholdRawRegMSB)
{
    uint8_t actualRawRegMSBThreshold = myfaultThresholdRegister->getThreshold();
    EXPECT_EQ(0, actualRawRegMSBThreshold);

    uint16_t thresholdValue = 0x1230;
    uint8_t expectedRawRegMSBThreshold = 0x24;
    myfaultThresholdRegister->setThreshold(thresholdValue);
    actualRawRegMSBThreshold = myfaultThresholdRegister->getThresholdRawRegMSB();
    EXPECT_EQ(expectedRawRegMSBThreshold, actualRawRegMSBThreshold);
}

TEST_F(faultThresholdRegisterTest, getThresholdRawRegLSB)
{
    uint8_t actualRawRegLSBThreshold = myfaultThresholdRegister->getThreshold();
    EXPECT_EQ(0, actualRawRegLSBThreshold);

    uint16_t thresholdValue = 0x1230;
    uint8_t expectedRawRegLSBThreshold = 0x60;
    myfaultThresholdRegister->setThreshold(thresholdValue);
    actualRawRegLSBThreshold = myfaultThresholdRegister->getThresholdRawRegLSB();
    EXPECT_EQ(expectedRawRegLSBThreshold, actualRawRegLSBThreshold);
}