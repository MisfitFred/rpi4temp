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

#ifdef MAX31865_COUT // save some space if we don't need this
void configRegister_t::print(void)
{
    std::cout << "vBias: " << vBias << std::endl;
    std::cout << "conversionMode: " << conversionMode << std::endl;
    std::cout << "oneShot: " << oneShot << std::endl;
    std::cout << "wire: " << wire << std::endl;
    std::cout << "faultDetection: " << faultDetection << std::endl;
    std::cout << "faultStatusClear: " << faultStatusClear << std::endl;
    std::cout << "filter: " << filter << std::endl;
}
#endif

#if MAX31865_STRING // save some space if we don't need this
std::string configRegister_t::toString(void)
{
    std::stringstream ss;
    std::string str;

    ss << "vBias: " << vBias << std::endl;
    ss << "conversionMode: " << conversionMode << std::endl;
    ss << "oneShot: " << oneShot << std::endl;
    ss << "wire: " << wire << std::endl;
    ss << "faultDetection: " << faultDetection << std::endl;
    ss << "faultStatusClear: " << faultStatusClear << std::endl;
    ss << "filter: " << filter << std::endl;
    str = ss.str();
    return str;
}
#endif

bool configRegister_t::operator==(const configRegister_t &c)
{
    if (this->vBias == c.vBias &&
        this->conversionMode == c.conversionMode &&
        this->oneShot == c.oneShot &&
        this->wire == c.wire &&
        this->faultDetection == c.faultDetection &&
        this->faultStatusClear == c.faultStatusClear &&
        this->filter == c.filter)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void configRegister_t::getRawValue(uint8_t *value)
{
    *value = 0;
    *value |= vBias << 7;
    *value |= conversionMode << 6;
    *value |= oneShot << 5;
    *value |= wire << 4;
    *value |= faultDetection << 2;
    *value |= faultStatusClear << 1;
    *value |= filter;
}

void configRegister_t::updateRawValue(uint8_t *config)
{
    vBias = static_cast<vBias_t>((config[0] >> 7) & 0x01);
    conversionMode = static_cast<conversionMode_t>((config[0] >> 6) & 0x01);
    oneShot = static_cast<oneShot_t>((config[0] >> 5) & 0x01);
    wire = static_cast<wire_t>((config[0] >> 4) & 0x01);
    faultDetection = static_cast<faultDetection_t>((config[0] >> 2) & 0x03);
    faultStatusClear = static_cast<faultStatusClear_t>((config[0] >> 1) & 0x01);
    filter = static_cast<filter_t>(config[0] & 0x01);
}

uint16_t rtdRegister_t::getResistance(void)
{
    return resistance;
}

bool rtdRegister_t::operator==(const rtdRegister_t &c)
{
    if (this->rtdFault == c.rtdFault &&
        this->resistance == c.resistance)
    {
        return true;
    }
    else
    {
        return false;
    }
}
#ifdef MAX31865_COUT
void rtdRegister_t::print(void)
{
    std::cout << "rtdFault: " << rtdFault << std::endl;
    std::cout << "resistance: " << resistance << std::endl;
}
#endif

#if 0
std::string rtdRegister_t::toString(void)
{
    std::stringstream ss;
    std::string str;

    ss << "rtdFault: " << rtdFault << std::endl;
    ss << "resistance: " << resistance << std::endl;
    str = ss.str();
    return str;
}
#endif
void rtdRegister_t::updateRawValue(uint8_t *value)
{
    uint16_t value_uint16 = (value[0] << 8) + value[1];
    this->resistance = value_uint16 >> 1;
    this->rtdFault = value_uint16 & 0x01;
};

uint16_t faultThresholdRegister_t::getThreshold(void)
{
    return threshold;
};

void faultThresholdRegister_t::setThreshold(uint16_t threshold)
{
    this->threshold = threshold;
};

#ifdef MAX31865_COUT
void faultThresholdRegister_t::print(void)
{
    std::cout << "threshold: " << threshold << std::endl;
}
#endif
#if 0
std::string faultThresholdRegister_t::toString(void)
{
    std::stringstream ss;
    std::string str;

    ss << "threshold: " << threshold << std::endl;
    str = ss.str();
    return str;
}
#endif

uint8_t faultThresholdRegister_t::getThresholdRawRegMSB(void)
{
    return (uint8_t)((threshold << 1 & 0xFF00) >> 8);
};

uint8_t faultThresholdRegister_t::getThresholdRawRegLSB(void)
{
    return (uint8_t)(threshold << 1 & 0x00FF);
};

void faultThresholdRegister_t::updateRawValue(uint8_t *value)
{
    uint16_t value_uint16 = (value[0] << 8) + value[1];
    this->threshold = value_uint16 >> 1;
};

void faultThresholdRegister_t::getRawValue(uint8_t *value)
{
    value[0] = getThresholdRawRegMSB();
    value[1] = getThresholdRawRegLSB();
};

bool faultThresholdRegister_t::operator==(const faultThresholdRegister_t &c)
{
    if (this->threshold == c.threshold)
    {
        return true;
    }
    else
    {
        return false;
    }
}

#ifdef MAX31865_COUT
void faultStatus_t::print(void)
{
    std::cout << "rtdHighThreshold: " << rtdHighThreshold << std::endl;
    std::cout << "rtdLowThreshold: " << rtdLowThreshold << std::endl;
    std::cout << "refInLowThreshold: " << refInLowThreshold << std::endl;
    std::cout << "refInHighThreshold: " << refInHighThreshold << std::endl;
    std::cout << "rtdInLowThreshold: " << rtdInLowThreshold << std::endl;
    std::cout << "overUnderVoltage: " << overUnderVoltage << std::endl;
}
#endif
#if 0
std::string faultStatus_t::toString(void)
{
    std::stringstream ss;
    std::string str;

    ss << "rtdHighThreshold: " << rtdHighThreshold << std::endl;
    ss << "rtdLowThreshold: " << rtdLowThreshold << std::endl;
    ss << "refInLowThreshold: " << refInLowThreshold << std::endl;
    ss << "refInHighThreshold: " << refInHighThreshold << std::endl;
    ss << "rtdInLowThreshold: " << rtdInLowThreshold << std::endl;
    ss << "overUnderVoltage: " << overUnderVoltage << std::endl;
    str = ss.str();
    return str;
}
#endif
void faultStatus_t::updateRawValue(uint8_t *value)
{
    rtdHighThreshold = value[0] & 0x80;
    rtdLowThreshold = value[0] & 0x40;
    refInHighThreshold = value[0] & 0x20;
    refInLowThreshold = value[0] & 0x10;
    rtdInLowThreshold = value[0] & 0x08;
    overUnderVoltage = value[0] & 0x04;
}

bool faultStatus_t::operator==(const faultStatus_t &c)
{
    if (this->rtdHighThreshold == c.rtdHighThreshold &&
        this->rtdLowThreshold == c.rtdLowThreshold &&
        this->refInHighThreshold == c.refInHighThreshold &&
        this->refInLowThreshold == c.refInLowThreshold &&
        this->rtdInLowThreshold == c.rtdInLowThreshold &&
        this->overUnderVoltage == c.overUnderVoltage)
    {
        return true;
    }
    else
    {
        return false;
    }
}