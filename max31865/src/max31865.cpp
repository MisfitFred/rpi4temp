#include "max31865.h"

max31865::max31865(spi *spiObj, uint8_t csPin, uint8_t rdyPin) : spiDevice(spiObj), readyPin(rdyPin), csPin(csPin){};

max31865::~max31865() {}

/**
 * @brief read register from max31865
 *
 * @param reg Register to read from, conten is written to reg
 *
 */
max31865::errorCode_t max31865::readRegister(max31865Register_t &reg)
{
    max31865::errorCode_t ret = NO_ERROR;
    if (max31865Register_t::WRITE_ONLY == reg.getRegPermission())
    {
        ret = INVALID_REGISTER_PERMISSION;
    }
    else
    {
        if (reg.length > 0)
        {
            spiData txData(reg.length + 1);
            spiData rxData(reg.length + 1);

            txData[0] = reg.baseAddress;

            spiDevice->transmit(&txData, &rxData, this->csPin);

            reg.updateRawValue(&(rxData[1]));

            ret = NO_ERROR;
        }
        else
        {
            ret = INVALID_REGISTER_LENGTH;
        }
    }
    return ret;
}

/**
 * @brief Write to a max31865 register
 *
 * @param reg Register to write to, reg the reg value is copied to there is no
 *            need to keep the reg in scope after the function returns
 *
 * @return max31865::errorCode_t
 */
max31865::errorCode_t max31865::writeRegister(max31865Register_t &reg)
{
    max31865::errorCode_t ret = NO_ERROR;

    if (max31865Register_t::READ_ONLY == reg.getRegPermission())
    {
        ret = INVALID_REGISTER_PERMISSION;
    }
    else
    {
        uint8_t regData[2];
        reg.getRawValue(regData);

        if (reg.length > 0)
        {
            spiData txData(reg.length + 1);
            txData[0] = reg.baseAddress | 0x80; // set write bit
            for (int i = 1; i < reg.length + 1; i++)
            {
                txData[i] = regData[i - 1];
            }

            spiDevice->transmit(&txData, nullptr, this->csPin);

            ret = NO_ERROR;
        }
        else
        {
            ret = INVALID_REGISTER_LENGTH;
        }
    }
    return ret;
}

max31865::errorCode_t max31865::testConnection(void)
{
    max31865::errorCode_t ret = NO_ERROR;
    configRegister_t writeConfig;
    configRegister_t readConfig;
    writeConfig.vBias = configRegister_t::vBias_t::ON;
    writeConfig.conversionMode = configRegister_t::conversionMode_t::AUTO;
    writeConfig.oneShot = configRegister_t::oneShot_t::SHOT_OFF;
    writeConfig.wire = configRegister_t::wire_t::WIRE_3;
    writeConfig.faultDetection = configRegister_t::faultDetection_t::NO_FAULT;
    writeConfig.faultStatusClear = configRegister_t::faultStatusClear_t::AUTO_CLEAR_FAULT;
    writeConfig.filter = configRegister_t::filter_t::FILTER_50HZ;

    writeRegister(writeConfig);
    readRegister(readConfig);

    if (writeConfig != readConfig)
    {
        ret = CONNECTION_ERROR;
    }

    faultHighThresholdRegister_t writeUpperThreshold;
    faultHighThresholdRegister_t readUpperThreshold;
    writeUpperThreshold.setThreshold(0x7FFF);
    writeRegister(writeUpperThreshold);
    readRegister(readUpperThreshold);

    if (writeUpperThreshold != readUpperThreshold)
    {
        ret = CONNECTION_ERROR;
    }

    faultLowThresholdRegister_t writeLowThreshold;
    faultLowThresholdRegister_t readLowThreshold;
    writeLowThreshold.setThreshold(0x0001);
    writeRegister(writeLowThreshold);
    readRegister(readLowThreshold);

    if (writeLowThreshold != readLowThreshold)
    {
        ret = CONNECTION_ERROR;
    }

    return ret;
}

max31865::errorCode_t max31865::readTemperature(float &temperature)
{
    errorCode_t ret = NO_ERROR;
    if (isConversionResultReady())
    {
        rtdRegister_t rtdReg;
        readRegister(rtdReg);

        if (nullptr == sensor)
        {
            ret = SENSOR_TYPE_NOT_SET;
        }
        else
        {
            temperature = sensor->convertTemperature(rtdReg.getResistance());
        }
    }
    else
    {
        ret = CONVERSION_NOT_READY;
    }
    return ret;
}

max31865::errorCode_t max31865::readFaultStatus(faultStatus_t &faultStatus)
{
    errorCode_t ret = NO_ERROR;
    readRegister(faultStatus);

    return ret;
}

float  max31865::convertTemperature(uint16_t resistorValue)
{
    float temperature = 0.0f;

    return sensor->convertTemperature(resistorValue);
}


max31865::errorCode_t max31865::startOneShotConversion(void)
{
    errorCode_t ret = NO_ERROR;
    configRegister_t config;
    readRegister(config);

    //@todo: Maybe it makes no sense to check for the ONE_SHOT mode
    if (config.oneShot == configRegister_t::oneShot_t::SHOT_ON)
    {
        ret = CONVERSION_ALREADY_RUNNING;
    }
    else if (config.conversionMode == configRegister_t::conversionMode_t::AUTO)
    {
        ret = CONVERSION_STILL_RUNNING;
    }
    {
        config.conversionMode = configRegister_t::conversionMode_t::MANUAL;
        config.oneShot = configRegister_t::oneShot_t::SHOT_ON;
        writeRegister(config);
        ret = NO_ERROR;
    }
    return ret;
}

max31865::errorCode_t max31865::startContinuousConversion(void)
{
    errorCode_t ret = NO_ERROR;
    configRegister_t config;
    readRegister(config);
    if (config.conversionMode == configRegister_t::conversionMode_t::AUTO)
    {
        ret = CONVERSION_ALREADY_RUNNING;
    }
    else
    {
        config.conversionMode = configRegister_t::conversionMode_t::AUTO;
        writeRegister(config);
        ret = NO_ERROR;
    }
    return ret;
}

max31865::errorCode_t max31865::stopContinuousConversion(void)
{
    errorCode_t ret = NO_ERROR;
    configRegister_t config;
    readRegister(config);

    config.conversionMode = configRegister_t::conversionMode_t::MANUAL;
    writeRegister(config);
    ret = NO_ERROR;

    return ret;
}

bool max31865::isConversionResultReady(void)
{
    return gpio_get(this->readyPin);
}
