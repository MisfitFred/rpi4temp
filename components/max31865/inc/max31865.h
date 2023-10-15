#pragma once
#include <string>
// #include <iostream>
// #include <sstream>

#include "spi.h"
// #include "log.h"
#include "max31865_reg.h"
#include "max31865_conv.h"

struct max31865Config
{
    spi *spiObj;
    uint8_t csPin;
    uint8_t rdyPin;
    bool threeWireMode;
    uint8_t upperThresholdInPercent;
    uint8_t lowerThresholdInPercent;
    max31865_sensor_t *sensorType;
    bool filter50Hz; // if false  60Hz
    bool faultDetectionOn;
};

/**
 * @brief Class for the max31865 temperature sensor
 *
 */
class max31865
{
public:
    enum errorCode_t
    {
        NO_ERROR = 0,
        CONVERSION_ALREADY_RUNNING,
        CONVERSION_STILL_RUNNING,
        CONVERSION_NOT_RUNNING,
        CONVERSION_NOT_READY,
        INVALID_REGISTER_LENGTH,
        INVALID_REGISTER_PERMISSION,
        CONNECTION_ERROR,
        SENSOR_TYPE_NOT_SET,
        CONFIGURATION_FAILED
    };

    max31865(spi *spiObj, uint8_t csPin, uint8_t rdyPin);

    max31865(max31865Config *config);
    max31865(){};
    ~max31865();

    errorCode_t readTemperature(float &temperature);
    errorCode_t readFaultStatus(faultStatus_t &faultStatus);
    errorCode_t clearFaultStatus(void);

    errorCode_t startOneShotConversion(void);
    errorCode_t startContinuousConversion(void);    
    errorCode_t stopContinuousConversion(void);

    /**
     * @brief Test the connection to the max31865
     *
     * @return errorCode_t
     */
    errorCode_t testConnection(void);

    bool isContinuousConversionRunning(void);
    bool isConversionResultReady(void);

   
    max31865::errorCode_t setUpperThresholdInPercent(uint8_t thresholdInPercent);
    max31865::errorCode_t setLowerThresholdInPercent(uint8_t thresholdInPercent);

    void setSensorType(max31865_sensor_t *sensorType) { sensor = sensorType; };

private:
    errorCode_t readRegister(max31865Register_t &reg);
    errorCode_t writeRegister(max31865Register_t &reg);
    void setConfig(max31865Config *config);
    
    errorCode_t writeConfigRegisters(void);

    bool thresholdChanged = true;
    bool conversionRunning = false;


    float convertTemperature(uint16_t resistorValue);
    spi *spiDevice;
    uint8_t readyPin;
    uint8_t csPin;
    configRegister_t configRegister;
    max31865_sensor_t *sensor = nullptr;

    faultHighThresholdRegister_t faultHighThresholdRegister;
    faultLowThresholdRegister_t faultLowThresholdRegister;
};
