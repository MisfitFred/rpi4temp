#include "max31865.h"
#include "spiMock.h"

#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace ::testing;
class max31865Test : public ::testing::Test
{
protected:
    max31865 *myMax31865;
    mockSpi *spiMock;

    virtual void SetUp()
    {
        spiMock = new mockSpi();
        myMax31865 = new max31865(spiMock, 0x55);
    }

    virtual void TearDown()
    {
        delete myMax31865;
        delete spiMock;
    }
};

TEST_F(max31865Test, readRegister)
{

    configRegister_t confReg;

    spiData *expTxData = new spiData(2);
    (*expTxData)[0] = 0x00;
    (*expTxData)[0] = 0x01;

    EXPECT_CALL(*spiMock, transmit(expTxData, nullptr, 2)).Times(1);
    myMax31865->readRegister(confReg);
}