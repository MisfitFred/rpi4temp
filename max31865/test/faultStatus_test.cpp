#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define UNIT_TEST
#include "max31865.h"
class faultStatusTest : public ::testing::Test
{
protected:
    faultStatus_t *myFaultStatus;
    virtual void SetUp() { myFaultStatus = new faultStatus_t(); }

    virtual void TearDown() { delete myFaultStatus; }

};

TEST_F(faultStatusTest, updateFaultStatus)
{
    uint8_t rawValue = 0x80;
    myFaultStatus->updateRawValue(&rawValue);
    EXPECT_EQ(myFaultStatus->rtdHighThreshold, true);
    EXPECT_EQ(myFaultStatus->rtdLowThreshold, false);
    EXPECT_EQ(myFaultStatus->refInHighThreshold, false);
    EXPECT_EQ(myFaultStatus->refInLowThreshold, false);
    EXPECT_EQ(myFaultStatus->rtdInLowThreshold, false);
    EXPECT_EQ(myFaultStatus->overUnderVoltage, false);

    rawValue = 0x40;
    myFaultStatus->updateRawValue(&rawValue);
    EXPECT_EQ(myFaultStatus->rtdHighThreshold, false);
    EXPECT_EQ(myFaultStatus->rtdLowThreshold, true);
    EXPECT_EQ(myFaultStatus->refInHighThreshold, false);
    EXPECT_EQ(myFaultStatus->refInHighThreshold, false);
    EXPECT_EQ(myFaultStatus->rtdInLowThreshold, false);
    EXPECT_EQ(myFaultStatus->overUnderVoltage, false);

    rawValue = 0x20;
    myFaultStatus->updateRawValue(&rawValue);
    EXPECT_EQ(myFaultStatus->rtdHighThreshold, false);
    EXPECT_EQ(myFaultStatus->rtdLowThreshold, false);
    EXPECT_EQ(myFaultStatus->refInHighThreshold, true);
    EXPECT_EQ(myFaultStatus->refInLowThreshold, false);
    EXPECT_EQ(myFaultStatus->rtdInLowThreshold, false);
    EXPECT_EQ(myFaultStatus->overUnderVoltage, false);

    rawValue = 0x10;
    myFaultStatus->updateRawValue(&rawValue);
    EXPECT_EQ(myFaultStatus->rtdHighThreshold, false);
    EXPECT_EQ(myFaultStatus->rtdLowThreshold, false);
    EXPECT_EQ(myFaultStatus->refInHighThreshold, false);
    EXPECT_EQ(myFaultStatus->refInLowThreshold, true);
    EXPECT_EQ(myFaultStatus->rtdInLowThreshold, false);
    EXPECT_EQ(myFaultStatus->overUnderVoltage, false);

    rawValue = 0x08;
    myFaultStatus->updateRawValue(&rawValue);
    EXPECT_EQ(myFaultStatus->rtdHighThreshold, false);
    EXPECT_EQ(myFaultStatus->rtdLowThreshold, false);
    EXPECT_EQ(myFaultStatus->refInHighThreshold, false);
    EXPECT_EQ(myFaultStatus->refInLowThreshold, false);
    EXPECT_EQ(myFaultStatus->rtdInLowThreshold, true);
    EXPECT_EQ(myFaultStatus->overUnderVoltage, false);
    
    rawValue = 0x04;
    myFaultStatus->updateRawValue(&rawValue);
    EXPECT_EQ(myFaultStatus->rtdHighThreshold, false);
    EXPECT_EQ(myFaultStatus->rtdLowThreshold, false);
    EXPECT_EQ(myFaultStatus->refInHighThreshold, false);
    EXPECT_EQ(myFaultStatus->refInLowThreshold, false);
    EXPECT_EQ(myFaultStatus->rtdInLowThreshold, false);
    EXPECT_EQ(myFaultStatus->overUnderVoltage, true);

    rawValue = 0x00;
    myFaultStatus->updateRawValue(&rawValue);
    EXPECT_EQ(myFaultStatus->rtdHighThreshold, false);
    EXPECT_EQ(myFaultStatus->rtdLowThreshold, false);
    EXPECT_EQ(myFaultStatus->refInHighThreshold, false);
    EXPECT_EQ(myFaultStatus->refInLowThreshold, false);
    EXPECT_EQ(myFaultStatus->rtdInLowThreshold, false);
    EXPECT_EQ(myFaultStatus->overUnderVoltage, false);
}
