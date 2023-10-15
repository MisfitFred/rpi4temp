#include "max31865.h"

max31865::~max31865() {}

max31865::max31865(max31865Config *config)
{
    setConfig(config);
}

void max31865::setConfig(max31865Config *config)
{
    this->spiDevice = config->spiObj;
    this->csPin = config->csPin;
    this->spiDevice->setMode(SPI_MODE_1);
    this->readyPin = config->rdyPin;
    this->sensor = config->sensorType;
    this->configRegister.wire = config->threeWireMode ? configRegister_t::wire_t::WIRE_3 : configRegister_t::wire_t::WIRE_2;
    this->configRegister.filter = config->filter50Hz ? configRegister_t::filter_t::FILTER_50HZ : configRegister_t::filter_t::FILTER_60HZ;

    //@todo: fault detection is not implemented yet
    this->configRegister.faultDetection = configRegister_t::faultDetection_t::AUTOMATIC_DELAY;
    this->configRegister.faultStatusClear = configRegister_t::faultStatusClear_t::AUTO_CLEAR_FAULT;
    
    this->configRegister.conversionMode = configRegister_t::conversionMode_t::MANUAL;
    this->configRegister.oneShot = configRegister_t::oneShot_t::SHOT_OFF;
    this->configRegister.vBias = configRegister_t::vBias_t::OFF;

    this->faultHighThresholdRegister.setThreshold(0x7FFF - (0x7FFF * config->upperThresholdInPercent / 100));
    this->faultLowThresholdRegister.setThreshold(0x7FFF * config->lowerThresholdInPercent / 100);
    this->thresholdChanged = true;
}

max31865::errorCode_t max31865::writeConfigRegisters(void)
{

    errorCode_t ret_config = writeRegister(this->configRegister);
    errorCode_t ret_highThreshold = NO_ERROR;
    errorCode_t ret_lowThreshold = NO_ERROR;

    if (this->thresholdChanged)
    {
        ret_highThreshold = writeRegister(this->faultHighThresholdRegister);
        ret_lowThreshold = writeRegister(this->faultLowThresholdRegister);
        this->thresholdChanged = false;
    }

    if (ret_config != NO_ERROR || ret_highThreshold != NO_ERROR || ret_lowThreshold != NO_ERROR)
    {
        return CONFIGURATION_FAILED;
    }
    else
    {
        return NO_ERROR;
    }
}

/**
 * @brief read register from max31865
 *
 * @param reg Register to read from, content is written to reg
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

max31865::errorCode_t max31865::clearFaultStatus(void)
{
    errorCode_t ret = NO_ERROR;
    configRegister_t config;
    readRegister(config);
    config.faultStatusClear = configRegister_t::faultStatusClear_t::AUTO_CLEAR_FAULT;
    writeRegister(config);
    return ret;
}

max31865::errorCode_t max31865::setUpperThresholdInPercent(uint8_t thresholdInPercent)
{
    errorCode_t ret = NO_ERROR;
    if (thresholdInPercent > 100)
    {
        ret = INVALID_REGISTER_LENGTH;
    }
    else
    {
        uint16_t threshold = 0x7FFF * thresholdInPercent / 100;
        faultHighThresholdRegister_t upperThreshold;
        upperThreshold.setThreshold(0x7FFF - threshold);
        writeRegister(upperThreshold);
    }
    this->thresholdChanged = true;

    return ret;
}

max31865::errorCode_t max31865::setLowerThresholdInPercent(uint8_t thresholdInPercent)
{
    errorCode_t ret = NO_ERROR;
    if (thresholdInPercent > 100)
    {
        ret = INVALID_REGISTER_LENGTH;
    }
    else
    {
        uint16_t threshold = 0x7FFF * thresholdInPercent / 100;
        faultLowThresholdRegister_t lowerThreshold;
        lowerThreshold.setThreshold(threshold);
        writeRegister(lowerThreshold);
    }
    this->thresholdChanged = true;

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
    writeConfig.faultStatusClear = configRegister_t::faultStatusClear_t::NO_CLEAR_FAULT;
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

    thresholdChanged = true;
    writeConfigRegisters();

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

float max31865::convertTemperature(uint16_t resistorValue)
{
    float temperature = 0.0f;

    return sensor->convertTemperature(resistorValue);
}

max31865::errorCode_t max31865::startOneShotConversion(void)
{

    this->configRegister.conversionMode = configRegister_t::conversionMode_t::MANUAL;
    this->configRegister.oneShot = configRegister_t::oneShot_t::SHOT_ON;
    this->configRegister.vBias = configRegister_t::vBias_t::ON;
    return writeConfigRegisters();

    conversionRunning = false; // @todo check if this is really correct
}

bool max31865::isContinuousConversionRunning(void)
{
    return this->conversionRunning;
}

max31865::errorCode_t max31865::startContinuousConversion(void)
{

    this->configRegister.conversionMode = configRegister_t::conversionMode_t::AUTO;
    this->configRegister.oneShot = configRegister_t::oneShot_t::SHOT_OFF;
    this->configRegister.vBias = configRegister_t::vBias_t::ON;
    conversionRunning = true;
    return writeConfigRegisters();
}

max31865::errorCode_t max31865::stopContinuousConversion(void)
{
    errorCode_t ret = NO_ERROR;
    configRegister_t config;
    readRegister(config);

    config.conversionMode = configRegister_t::conversionMode_t::MANUAL;
    config.vBias = configRegister_t::vBias_t::OFF;

    conversionRunning = false;
    return writeRegister(config);
}

bool max31865::isConversionResultReady(void)
{
    return !gpio_get(this->readyPin);
}
