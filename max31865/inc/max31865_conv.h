#pragma once

#include <cstdint>
/**
 * @brief abstract class for temperature conversion
 *
 */
class max31865_sensor_t
{
public:
    /**
     * @brief Convert the temperature to a float value
     *
     * @param adc
     * @param refResistor
     * @return float
     */
    virtual float convertTemperature(uint16_t adc) = 0;
};

/**
 * @brief class for temperature conversion of pt100 optimized for the range between 50°C and 120°C
 *
 */
class pt100_optimized50to120 : public max31865_sensor_t
{
public:
    /**
     * @brief Convert the temperature to a float value
     *
     * @param adc
     * @param refResistor
     * @return float
     */
    float convertTemperature(uint16_t adc);

    uint32_t R0 = 420;
};


/**
 * @brief class for temperature conversion of pt100 optimized for the range between 50°C and 120°C
 *
 */
class pt100 : public max31865_sensor_t
{
public:
    /**
     * @brief Convert the temperature to a float value
     *
     * @param adc
     * @param refResistor
     * @return float
     */
    float convertTemperature(uint16_t adc);

    uint32_t R0 = 420;
};