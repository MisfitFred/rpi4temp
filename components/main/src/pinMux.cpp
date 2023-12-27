
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#include "pinMux.h"

static void initCsPin(uint8_t pin, bool init)
{
    // set pin mux for SPI
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, init); // cs is active low, initialize as high
}

static void initRdyPin(uint8_t pin)
{
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
}

/**
 * @brief initialize the pin mux
 * 
 */
void init_PinMux(void)
{

    initCsPin(SPI0_CS0_PIN, 1);
    bi_decl(bi_1pin_with_name(SPI0_CS0_PIN, "SPI0 CS0"));
    initCsPin(SPI0_CS1_PIN, 1);
    bi_decl(bi_1pin_with_name(SPI0_CS1_PIN, "SPI0 CS1"));
    initCsPin(SPI0_CS2_PIN, 1);
    bi_decl(bi_1pin_with_name(SPI0_CS2_PIN, "SPI0 CS2"));
    initCsPin(SPI0_CS3_PIN, 1);
    bi_decl(bi_1pin_with_name(SPI0_CS3_PIN, "SPI0 CS3"));

    initRdyPin(RDY0_PIN);
    bi_decl(bi_1pin_with_name(RDY0_PIN, "RDY0"));
    initRdyPin(RDY1_PIN);
    bi_decl(bi_1pin_with_name(RDY1_PIN, "RDY1"));
    initRdyPin(RDY2_PIN);
    bi_decl(bi_1pin_with_name(RDY2_PIN, "RDY2"));
    initRdyPin(RDY3_PIN);
    bi_decl(bi_1pin_with_name(RDY3_PIN, "RDY3"));

    // set pin mux for SPI
    gpio_set_function(SPI0_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_TX_PIN, GPIO_FUNC_SPI);

    bi_decl(bi_3pins_with_func(SPI0_RX_PIN, SPI0_TX_PIN, SPI0_SCK_PIN, GPIO_FUNC_SPI));
}