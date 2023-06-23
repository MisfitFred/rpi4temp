

#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define UNIT_TEST
#include "max31865.h"

class rtdRegisterTest : public ::testing::Test
{
protected:
    rtdRegister_t *myRtdRegister;
    virtual void SetUp() { myRtdRegister = new rtdRegister_t(); }

    virtual void TearDown() { delete myRtdRegister; }
};

TEST_F(rtdRegisterTest, update_get_Resistance)
{
    uint16_t actualResistance = myRtdRegister->getResistance();
    EXPECT_EQ(0, actualResistance);

    uint16_t expectedResistance = 0x919; 
    uint8_t registerRawValue_uint8p[2] = {0x12,0x32}; //0x1232
    myRtdRegister->updateRawValue(registerRawValue_uint8p);
    actualResistance = myRtdRegister->getResistance();
    EXPECT_EQ(expectedResistance, actualResistance);
}

TEST_F(rtdRegisterTest, rtdFault)
{
    bool actualRtdFault = myRtdRegister->rtdFault;
    EXPECT_FALSE(actualRtdFault);

    uint8_t expectedResistance_uint8p[2] = {0xFF,0xFE};
    myRtdRegister->updateRawValue(expectedResistance_uint8p);
    actualRtdFault = myRtdRegister->rtdFault;
    EXPECT_FALSE(actualRtdFault);

    expectedResistance_uint8p[0] = 0x00;
    expectedResistance_uint8p[1] = 0x01;
    myRtdRegister->updateRawValue(expectedResistance_uint8p);
    actualRtdFault = myRtdRegister->rtdFault;
    EXPECT_TRUE(actualRtdFault);
}

TEST_F(rtdRegisterTest, print)
{
    uint8_t expectedResistance_uint8p[2] = {0x12,0x34}; //0x1234
    myRtdRegister->updateRawValue(expectedResistance_uint8p);
    testing::internal::CaptureStdout();
    myRtdRegister->print();
    std::string actualPrint = testing::internal::GetCapturedStdout();
    std::string expectedPrint =
        "rtdFault: 0\n"
        "resistance: 2330\n";
    EXPECT_EQ(expectedPrint, actualPrint);
}

TEST_F(rtdRegisterTest, toString)
{
    uint8_t expectedResistance_uint8p[2] = {0x12,0x34}; //0x1234;

    myRtdRegister->updateRawValue(expectedResistance_uint8p);
    std::string actualToString = myRtdRegister->toString();
    std::string expectedToString =
        "rtdFault: 0\n"
        "resistance: 2330\n";
    EXPECT_EQ(expectedToString, actualToString);
}
