#include "max31865.h"


max31865::max31865(spi *spiObj, uint8_t rdyPin) : spiDevice(spiObj), readyPin(rdyPin)
{
    spiDevice->openDevice();
    spiDevice->setMode();
    spiDevice->setSpeed();
};

max31865::~max31865()
{
    spiDevice->closeDevice();
}

max31865::errorCode_t max31865::readRegister(max31865Register_t reg)
{
    max31865::errorCode_t ret = NO_ERROR;
    uint8_t regData[2];

    
    reg.getRawValue(regData);

    if (reg.length > 0)
    {
        spiData txData(reg.length + 1);
        spiData rxData(reg.length + 1);

        txData[0] = reg.baseAddress;
        
        spiDevice->transmit(&txData, &rxData, reg.length+1);

        for (int i = 1; i < reg.length + 1; i++)
        {
            reg.updateRawValue(&(rxData[0]));
        }
        ret = NO_ERROR;
    }
    else
    {
        ret = INVALID_REGISTER_LENGTH;
    }

    return ret;
}

max31865::errorCode_t max31865::writeRegister(max31865Register_t reg)
{
    max31865::errorCode_t ret = NO_ERROR;
    uint8_t regData[2];
    reg.getRawValue(regData);

    if (reg.length > 0)
    {
        spiData txData(reg.length + 1);
        txData[0] = reg.baseAddress | 0x80;
        for (int i = 1; i < reg.length + 1; i++)
        {
            txData[i] = regData[i];
        }

        spiDevice->transmit(&txData, nullptr, reg.length+1);

        ret = NO_ERROR;
    }
    else
    {
        ret = INVALID_REGISTER_LENGTH;
    }

    return ret;
}

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

void rtdRegister_t::print(void)
{
    std::cout << "rtdFault: " << rtdFault << std::endl;
    std::cout << "resistance: " << resistance << std::endl;
}

std::string rtdRegister_t::toString(void)
{
    std::stringstream ss;
    std::string str;

    ss << "rtdFault: " << rtdFault << std::endl;
    ss << "resistance: " << resistance << std::endl;
    str = ss.str();
    return str;
}

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

void faultThresholdRegister_t::print(void)
{
    std::cout << "threshold: " << threshold << std::endl;
}

std::string faultThresholdRegister_t::toString(void)
{
    std::stringstream ss;
    std::string str;

    ss << "threshold: " << threshold << std::endl;
    str = ss.str();
    return str;
}

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

void faultStatus_t::print(void)
{
    std::cout << "rtdHighThreshold: " << rtdHighThreshold << std::endl;
    std::cout << "rtdLowThreshold: " << rtdLowThreshold << std::endl;
    std::cout << "refInLowThreshold: " << refInLowThreshold << std::endl;
    std::cout << "refInHighThreshold: " << refInHighThreshold << std::endl;
    std::cout << "rtdInLowThreshold: " << rtdInLowThreshold << std::endl;
    std::cout << "overUnderVoltage: " << overUnderVoltage << std::endl;
}

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

void faultStatus_t::updateRawValue(uint8_t *value)
{
    rtdHighThreshold = value[0] & 0x80;
    rtdLowThreshold = value[0] & 0x40;
    refInHighThreshold = value[0] & 0x20;
    refInLowThreshold = value[0] & 0x10;
    rtdInLowThreshold = value[0] & 0x08;
    overUnderVoltage = value[0] & 0x04;
}
