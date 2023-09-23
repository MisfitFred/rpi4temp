
#include <cstdint>
#include "hardware/gpio.h"

struct pinDefRP2040_t
{
    uint8_t pinNumber;
    gpio_function function;
    bool isOutPin;
    bool outValue;
};

extern pinDefRP2040_t pinMuxSetRP2040_oneMAX31865[5];

void pinMux_init(pinDefRP2040_t *mux, uint32_t numOfPins);
