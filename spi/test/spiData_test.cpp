#include "spi.h"

#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

/**
    spiData(uint32_t size);
    ~spiData();
    uint8_t *getBufferP(void);
    uint32_t getBufferSize(void);
    unsigned long getBufferAdr(void);
    constexpr uint8_t &operator[](uint32_t index) noexcept;
    constexpr const uint8_t &operator[](uint32_t index) const noexcept;
    std::string to_string(void);
    void print(void);
*/
#include <array> 


class spiDataTest : public ::testing::Test
{
protected:
    spiData *mySpiData = nullptr;
    virtual void SetUp() {}

    virtual void TearDown()
    {
        if (mySpiData != nullptr)
            delete mySpiData;
    }
};

/**
 * @brief Test the creation of spiData class
 *        checks buffer size and initial values
*/
TEST_F(spiDataTest, create)
{
    mySpiData = new spiData(32);
    EXPECT_THAT(32, mySpiData->getBufferSize());
    for (int i=0; i<32; i++)
        EXPECT_THAT(0, mySpiData->getBufferP()[i]);
    EXPECT_THAT(mySpiData->getBufferAdr(), (unsigned long)mySpiData->getBufferP());
}

/**
 * @brief Test the operator[] of spiData class
*/
TEST_F(spiDataTest, operator1)
{
    mySpiData = new spiData(32);
    EXPECT_THAT(32, mySpiData->getBufferSize());

    mySpiData[0]=1;
    EXPECT_THAT(1, mySpiData->getBufferP()[0]);
    for (int i=1; i<32; i++)
        EXPECT_THAT(0, mySpiData->getBufferP()[i]);
    EXPECT_THAT(mySpiData->getBufferAdr(), (unsigned long)mySpiData->getBufferP());
}