#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define UNIT_TEST
#include "max31865.h"

class configRegister_t_Test : public ::testing::Test
{
protected:
    configRegister_t *configRegister;
    virtual void SetUp() { configRegister = new configRegister_t(); }
    virtual void TearDown() { delete configRegister; }
};

TEST_F(configRegister_t_Test, DefaultConstructor)
{
    EXPECT_EQ(configRegister->vBias, configRegister_t::vBias_t::OFF);
    EXPECT_EQ(configRegister->conversionMode, configRegister_t::conversionMode_t::MANUAL);
    EXPECT_EQ(configRegister->oneShot, configRegister_t::oneShot_t::SHOT_OFF);
    EXPECT_EQ(configRegister->wire, configRegister_t::wire_t::WIRE_2);
    EXPECT_EQ(configRegister->faultDetection, configRegister_t::faultDetection_t::NO_FAULT);
    EXPECT_EQ(configRegister->faultStatusClear, configRegister_t::faultStatusClear_t::NO_CLEAR_FAULT);
    EXPECT_EQ(configRegister->filter, configRegister_t::filter_t::FILTER_60HZ);
}

TEST_F(configRegister_t_Test, GetRawValue)
{
    configRegister->vBias = configRegister_t::vBias_t::OFF;
    configRegister->conversionMode = configRegister_t::conversionMode_t::MANUAL;
    configRegister->oneShot = configRegister_t::oneShot_t::SHOT_OFF;
    configRegister->wire = configRegister_t::wire_t::WIRE_2;
    configRegister->faultDetection = configRegister_t::faultDetection_t::NO_FAULT;
    configRegister->faultStatusClear = configRegister_t::faultStatusClear_t::NO_CLEAR_FAULT;
    configRegister->filter = configRegister_t::filter_t::FILTER_60HZ;

    uint8_t rawValue = 0;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x00) << "Default config register value is not correct";

    configRegister->vBias = configRegister_t::vBias_t::ON;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x80) << " vBias ON value is not correct";
    configRegister->vBias = configRegister_t::vBias_t::OFF;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x00) << "vBias OFF value is not correct";

    configRegister->conversionMode = configRegister_t::conversionMode_t::AUTO;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x40) << "conversionMode AUTO value is not correct";
    configRegister->conversionMode = configRegister_t::conversionMode_t::MANUAL;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x00) << "conversionMode MANUAL value is not correct";

    configRegister->oneShot = configRegister_t::oneShot_t::SHOT_ON;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x20) << "oneShot ON value is not correct";
    configRegister->oneShot = configRegister_t::oneShot_t::SHOT_OFF;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x00) << "oneShot OFF value is not correct";

    configRegister->wire = configRegister_t::wire_t::WIRE_2;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x00) << "wire 2 value is not correct";
    configRegister->wire = configRegister_t::wire_t::WIRE_3;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x10) << "wire 3 value is not correct";
    configRegister->wire = configRegister_t::wire_t::WIRE_4;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x00) << "wire 4 value is not correct";

    configRegister->faultDetection = configRegister_t::faultDetection_t::FINISH_WITH_MANUAL_DELAY;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x0C) << "faultDetection FINISH_WITH_MANUAL_DELAY value is not correct";
    configRegister->faultDetection = configRegister_t::faultDetection_t::RUN_WITH_MANUAL_DELAY;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x08) << "faultDetection RUN_WITH_MANUAL_DELAY value is not correct";
    configRegister->faultDetection = configRegister_t::faultDetection_t::AUTOMATIC_DELAY;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x04) << "faultDetection AUTOMATIC_DELAY value is not correct";
    configRegister->faultDetection = configRegister_t::faultDetection_t::NO_FAULT;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x00) << "faultDetection NO_FAULT value is not correct";

    configRegister->faultStatusClear = configRegister_t::faultStatusClear_t::AUTO_CLEAR_FAULT;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x02) << "faultStatusClear AUTO_CLEAR_FAULT value is not correct";
    configRegister->faultStatusClear = configRegister_t::faultStatusClear_t::NO_CLEAR_FAULT;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x00) << "faultStatusClear NO_CLEAR_FAULT value is not correct";

    configRegister->filter = configRegister_t::filter_t::FILTER_50HZ;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x01) << "filter FILTER_50HZ value is not correct";
    configRegister->filter = configRegister_t::filter_t::FILTER_60HZ;
    configRegister->getRawValue(&rawValue);
    EXPECT_EQ(rawValue, 0x00) << "filter FILTER_60HZ value is not correct";
}

TEST_F(configRegister_t_Test, UpdateRawValue)
{
    uint8_t rawValue = 0x80;
    configRegister->updateRawValue(&rawValue);

    EXPECT_EQ(configRegister->vBias, configRegister_t::vBias_t::ON);
    EXPECT_EQ(configRegister->conversionMode, configRegister_t::conversionMode_t::MANUAL);
    EXPECT_EQ(configRegister->oneShot, configRegister_t::oneShot_t::SHOT_OFF);
    EXPECT_EQ(configRegister->wire, configRegister_t::wire_t::WIRE_2);
    EXPECT_EQ(configRegister->faultDetection, configRegister_t::faultDetection_t::NO_FAULT);
    EXPECT_EQ(configRegister->faultStatusClear, configRegister_t::faultStatusClear_t::NO_CLEAR_FAULT);
    EXPECT_EQ(configRegister->filter, configRegister_t::filter_t::FILTER_60HZ);

    rawValue = 0x40;
    configRegister->updateRawValue(&rawValue);

    EXPECT_EQ(configRegister->vBias, configRegister_t::vBias_t::OFF);
    EXPECT_EQ(configRegister->conversionMode, configRegister_t::conversionMode_t::AUTO);
    EXPECT_EQ(configRegister->oneShot, configRegister_t::oneShot_t::SHOT_OFF);
    EXPECT_EQ(configRegister->wire, configRegister_t::wire_t::WIRE_2);
    EXPECT_EQ(configRegister->faultDetection, configRegister_t::faultDetection_t::NO_FAULT);
    EXPECT_EQ(configRegister->faultStatusClear, configRegister_t::faultStatusClear_t::NO_CLEAR_FAULT);
    EXPECT_EQ(configRegister->filter, configRegister_t::filter_t::FILTER_60HZ);

    rawValue = 0x20;
    configRegister->updateRawValue(&rawValue);

    EXPECT_EQ(configRegister->vBias, configRegister_t::vBias_t::OFF);
    EXPECT_EQ(configRegister->conversionMode, configRegister_t::conversionMode_t::MANUAL);
    EXPECT_EQ(configRegister->oneShot, configRegister_t::oneShot_t::SHOT_ON);
    EXPECT_EQ(configRegister->wire, configRegister_t::wire_t::WIRE_2);
    EXPECT_EQ(configRegister->faultDetection, configRegister_t::faultDetection_t::NO_FAULT);
    EXPECT_EQ(configRegister->faultStatusClear, configRegister_t::faultStatusClear_t::NO_CLEAR_FAULT);
    EXPECT_EQ(configRegister->filter, configRegister_t::filter_t::FILTER_60HZ);

    rawValue = 0x10;
    configRegister->updateRawValue(&rawValue);

    EXPECT_EQ(configRegister->vBias, configRegister_t::vBias_t::OFF);
    EXPECT_EQ(configRegister->conversionMode, configRegister_t::conversionMode_t::MANUAL);
    EXPECT_EQ(configRegister->oneShot, configRegister_t::oneShot_t::SHOT_OFF);
    EXPECT_EQ(configRegister->wire, configRegister_t::wire_t::WIRE_3);
    EXPECT_EQ(configRegister->faultDetection, configRegister_t::faultDetection_t::NO_FAULT);
    EXPECT_EQ(configRegister->faultStatusClear, configRegister_t::faultStatusClear_t::NO_CLEAR_FAULT);
    EXPECT_EQ(configRegister->filter, configRegister_t::filter_t::FILTER_60HZ);

    rawValue = 0x08;
    configRegister->updateRawValue(&rawValue);

    EXPECT_EQ(configRegister->vBias, configRegister_t::vBias_t::OFF);
    EXPECT_EQ(configRegister->conversionMode, configRegister_t::conversionMode_t::MANUAL);
    EXPECT_EQ(configRegister->oneShot, configRegister_t::oneShot_t::SHOT_OFF);
    EXPECT_EQ(configRegister->wire, configRegister_t::wire_t::WIRE_2);
    EXPECT_EQ(configRegister->faultDetection, configRegister_t::faultDetection_t::RUN_WITH_MANUAL_DELAY);
    EXPECT_EQ(configRegister->faultStatusClear, configRegister_t::faultStatusClear_t::NO_CLEAR_FAULT);
    EXPECT_EQ(configRegister->filter, configRegister_t::filter_t::FILTER_60HZ);

    rawValue = 0x04;
    configRegister->updateRawValue(&rawValue);

    EXPECT_EQ(configRegister->vBias, configRegister_t::vBias_t::OFF);
    EXPECT_EQ(configRegister->conversionMode, configRegister_t::conversionMode_t::MANUAL);
    EXPECT_EQ(configRegister->oneShot, configRegister_t::oneShot_t::SHOT_OFF);
    EXPECT_EQ(configRegister->wire, configRegister_t::wire_t::WIRE_2);
    EXPECT_EQ(configRegister->faultDetection, configRegister_t::faultDetection_t::AUTOMATIC_DELAY);
    EXPECT_EQ(configRegister->faultStatusClear, configRegister_t::faultStatusClear_t::NO_CLEAR_FAULT);
    EXPECT_EQ(configRegister->filter, configRegister_t::filter_t::FILTER_60HZ);

    rawValue = 0x03;
    configRegister->updateRawValue(&rawValue);

    EXPECT_EQ(configRegister->vBias, configRegister_t::vBias_t::OFF);
    EXPECT_EQ(configRegister->conversionMode, configRegister_t::conversionMode_t::MANUAL);
    EXPECT_EQ(configRegister->oneShot, configRegister_t::oneShot_t::SHOT_OFF);
    EXPECT_EQ(configRegister->wire, configRegister_t::wire_t::WIRE_2);
    EXPECT_EQ(configRegister->faultDetection, configRegister_t::faultDetection_t::FINISH_WITH_MANUAL_DELAY);
    EXPECT_EQ(configRegister->faultStatusClear, configRegister_t::faultStatusClear_t::NO_CLEAR_FAULT);
    EXPECT_EQ(configRegister->filter, configRegister_t::filter_t::FILTER_60HZ);

    rawValue = 0x02;
    configRegister->updateRawValue(&rawValue);

    EXPECT_EQ(configRegister->vBias, configRegister_t::vBias_t::OFF);
    EXPECT_EQ(configRegister->conversionMode, configRegister_t::conversionMode_t::MANUAL);
    EXPECT_EQ(configRegister->oneShot, configRegister_t::oneShot_t::SHOT_OFF);
    EXPECT_EQ(configRegister->wire, configRegister_t::wire_t::WIRE_2);
    EXPECT_EQ(configRegister->faultDetection, configRegister_t::faultDetection_t::NO_FAULT);
    EXPECT_EQ(configRegister->faultStatusClear, configRegister_t::faultStatusClear_t::AUTO_CLEAR_FAULT);
    EXPECT_EQ(configRegister->filter, configRegister_t::filter_t::FILTER_60HZ);

    rawValue = 0x01;
    configRegister->updateRawValue(&rawValue);

    EXPECT_EQ(configRegister->vBias, configRegister_t::vBias_t::OFF);
    EXPECT_EQ(configRegister->conversionMode, configRegister_t::conversionMode_t::MANUAL);
    EXPECT_EQ(configRegister->oneShot, configRegister_t::oneShot_t::SHOT_OFF);
    EXPECT_EQ(configRegister->wire, configRegister_t::wire_t::WIRE_2);
    EXPECT_EQ(configRegister->faultDetection, configRegister_t::faultDetection_t::NO_FAULT);
    EXPECT_EQ(configRegister->faultStatusClear, configRegister_t::faultStatusClear_t::NO_CLEAR_FAULT);
    EXPECT_EQ(configRegister->filter, configRegister_t::filter_t::FILTER_50HZ);
}

TEST_F(configRegister_t_Test, Print)
{
    configRegister->vBias = configRegister_t::vBias_t::ON;
    configRegister->conversionMode = configRegister_t::conversionMode_t::AUTO;
    configRegister->oneShot = configRegister_t::oneShot_t::SHOT_OFF;
    configRegister->wire = configRegister_t::wire_t::WIRE_3;
    configRegister->faultDetection = configRegister_t::faultDetection_t::NO_FAULT;
    configRegister->faultStatusClear = configRegister_t::faultStatusClear_t::NO_CLEAR_FAULT;
    configRegister->filter = configRegister_t::filter_t::FILTER_60HZ;

    std::stringstream ss;
    std::string str;
    ss << "vBias: " << configRegister->vBias << std::endl;
    ss << "conversionMode: " << configRegister->conversionMode << std::endl;
    ss << "oneShot: " << configRegister->oneShot << std::endl;
    ss << "wire: " << configRegister->wire << std::endl;
    ss << "faultDetection: " << configRegister->faultDetection << std::endl;
    ss << "faultStatusClear: " << configRegister->faultStatusClear << std::endl;
    ss << "filter: " << configRegister->filter << std::endl;
    str = ss.str();

    testing::internal::CaptureStdout();
    configRegister->print();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, str);
}

TEST_F(configRegister_t_Test, toString)
{
    configRegister->vBias = configRegister_t::vBias_t::ON;
    configRegister->conversionMode = configRegister_t::conversionMode_t::AUTO;
    configRegister->oneShot = configRegister_t::oneShot_t::SHOT_OFF;
    configRegister->wire = configRegister_t::wire_t::WIRE_3;
    configRegister->faultDetection = configRegister_t::faultDetection_t::NO_FAULT;
    configRegister->faultStatusClear = configRegister_t::faultStatusClear_t::NO_CLEAR_FAULT;
    configRegister->filter = configRegister_t::filter_t::FILTER_60HZ;

    std::stringstream ss;
    std::string str;
    ss << "vBias: " << configRegister->vBias << std::endl;
    ss << "conversionMode: " << configRegister->conversionMode << std::endl;
    ss << "oneShot: " << configRegister->oneShot << std::endl;
    ss << "wire: " << configRegister->wire << std::endl;
    ss << "faultDetection: " << configRegister->faultDetection << std::endl;
    ss << "faultStatusClear: " << configRegister->faultStatusClear << std::endl;
    ss << "filter: " << configRegister->filter << std::endl;
    str = ss.str();

    EXPECT_EQ(configRegister->toString(), str);
}