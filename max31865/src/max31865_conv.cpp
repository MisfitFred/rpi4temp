#include "max31865_conv.h"
#include <cmath>
#include <numbers>


float pt100_optimized50to120::convertTemperature(uint16_t adc)
{
    float rtd = (float)adc * R0 / 32767.0f;
    float temperature = (rtd - 100.393f) / 0.3810125f;
    return temperature;

}


float pt100::convertTemperature(uint16_t adc)
{

    const float A = 3.9083e-3f;
    const float B = -5.775e-7f;

    float rtd = (float)adc * R0 / 32768.0f;
    // need to be checked
    float temperature = (-A * R0 + sqrt((A * R0) * (A * R0) - 4 * B * R0 * (R0 - rtd)))/(2 * (R0 - rtd));
    return temperature;
}
