#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "max31865.h"
#include "spi.h"

#include "FreeRTOS.h"
#include "task.h"
/*
 *  GPIO   | F1
 *  -------|----------
 *  2      | SPI0_SCK_PIN
 *  3      | SPI0_TX_PIN
 *  4      | RDY0_PIN
 *  5      | SPI0_RX_PIN
 *  6      | SPI0_CS0_PIN
 */

const uint8_t SPI0_SCK_PIN = 2;
const uint8_t SPI0_TX_PIN = 3;
const uint8_t SPI0_RX_PIN = 4;
const uint8_t SPI0_CS0_PIN = 5;
const uint8_t RDY0_PIN = 6;
// main function
int main(int argc __attribute__((unused)), char const *argv[] __attribute__((unused)))
{
    stdio_init_all();

    // set pin mux for SPI
    gpio_init(SPI0_CS0_PIN);
    gpio_set_dir(SPI0_CS0_PIN, GPIO_OUT);
    gpio_put(SPI0_CS0_PIN, 1); //cs is active low, initialize as high
    bi_decl(bi_1pin_with_name(SPI0_CS0_PIN, "SPI0 CS0"));

    gpio_init(RDY0_PIN);
    gpio_set_dir(RDY0_PIN, GPIO_IN);
    bi_decl(bi_1pin_with_name(RDY0_PIN, "RDY0"));

    gpio_set_function(SPI0_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_TX_PIN, GPIO_FUNC_SPI);
    bi_decl(bi_3pins_with_func(SPI0_RX_PIN, SPI0_TX_PIN, SPI0_SCK_PIN, GPIO_FUNC_SPI));

    spi *mySpiObj = spi::getInstance(0);
    mySpiObj->setMode(SPI_MODE_1);
    max31865 *myMax31865 = new max31865(mySpiObj, SPI0_CS0_PIN, RDY0_PIN);

    configRegister_t *writeConfigRegister_1 = new configRegister_t();
    writeConfigRegister_1->filter = configRegister_t::filter_t::FILTER_50HZ;

    configRegister_t *writeConfigRegister_2 = new configRegister_t();
    writeConfigRegister_2->vBias = configRegister_t::vBias_t::ON;
    writeConfigRegister_2->conversionMode = configRegister_t::conversionMode_t::AUTO;
    writeConfigRegister_2->oneShot = configRegister_t::oneShot_t::SHOT_OFF;
    writeConfigRegister_2->wire = configRegister_t::wire_t::WIRE_4;
    writeConfigRegister_2->filter = configRegister_t::filter_t::FILTER_50HZ;
    writeConfigRegister_2->faultDetection = configRegister_t::faultDetection_t::NO_FAULT;

    configRegister_t *readConfigRegister_1 = new configRegister_t();
    configRegister_t *readConfigRegister_2 = new configRegister_t();

    int i = 0;
    while (7)
    {
        myMax31865->writeRegister(*writeConfigRegister_1);
        myMax31865->readRegister(*readConfigRegister_1);
        myMax31865->writeRegister(*writeConfigRegister_2);
        myMax31865->readRegister(*readConfigRegister_2);
        // sleep for 100ms
        sleep_ms(100);
    }
}
