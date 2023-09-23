
#include "pinMux.h"



pinDefRP2040_t pinMuxSetRP2040_oneMAX31865[5] = {
    {.pinNumber = 2, .function = GPIO_FUNC_SPI, .isOutPin = false, .outValue = false},
    {.pinNumber = 3, .function = GPIO_FUNC_SPI, .isOutPin = false, .outValue = false},
    {.pinNumber = 4, .function = GPIO_FUNC_SPI, .isOutPin = false, .outValue = false},
    {.pinNumber = 5, .function = GPIO_FUNC_SIO, .isOutPin = false, .outValue = false},
    {.pinNumber = 6, .function = GPIO_FUNC_SIO, .isOutPin = false, .outValue = false}};

void pinMux_init(pinDefRP2040_t *mux, uint32_t numOfPins)
{
    for (int i = 0; i < numOfPins; i++)
    {
        switch (mux[i].function)
        {
        case GPIO_FUNC_SIO:
            gpio_set_dir(mux[i].pinNumber, mux[i].isOutPin);
            if (mux[i].isOutPin)
            {
                gpio_put(mux[i].pinNumber, mux[i].outValue);
            }
            else
            {
                gpio_put(mux[i].pinNumber, 0);
            }
            gpio_set_function(mux[i].pinNumber, GPIO_FUNC_SIO);
            break;
        case GPIO_FUNC_SPI:
            gpio_set_function(mux[i].pinNumber, GPIO_FUNC_SPI);
            break;
        default:
            break;
        }
    }
}
