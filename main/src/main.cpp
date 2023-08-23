#include <string.h>

#include "pico/stdlib.h"

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
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

void chipSelect(uint8_t csPin)
{
    asm volatile("nop \n nop \n nop");
    gpio_put(csPin, 0);
    asm volatile("nop \n nop \n nop");
}

void chipDeselect(uint8_t csPin)
{
    asm volatile("nop \n nop \n nop");
    gpio_put(csPin, 1);
    asm volatile("nop \n nop \n nop");
}

// main function
int main(int argc __attribute__((unused)), char const *argv[] __attribute__((unused)))
{
    stdio_init_all();

    // set pin mux for SPI
    gpio_init(RDY0_PIN);
    gpio_set_dir(SPI0_CS0_PIN, GPIO_OUT);
    gpio_put(SPI0_CS0_PIN, 1); // cs is active low, initialize as high
    bi_decl(bi_1pin_with_name(SPI0_CS0_PIN, "SPI0 CS0"));

    gpio_init(RDY0_PIN);
    gpio_set_dir(RDY0_PIN, GPIO_IN);
    bi_decl(bi_1pin_with_name(RDY0_PIN, "RDY0"));

    gpio_set_function(SPI0_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_TX_PIN, GPIO_FUNC_SPI);
    bi_decl(bi_3pins_with_func(SPI0_RX_PIN, SPI0_TX_PIN, SPI0_SCK_PIN, GPIO_FUNC_SPI));

    uint8_t txBuffer1[7] = {0x80, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t rxBuffer1[7] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    uint8_t txBuffer2[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t rxBuffer2[7] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    uint32_t spiSpeedReal = spi_init(spi0, 20000);
    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);


    while (1)
    {
        int ret = 0;

        chipSelect(SPI0_CS0_PIN);
        ret = spi_write_read_blocking(spi0, txBuffer1, rxBuffer1, 2);
        chipDeselect(SPI0_CS0_PIN);

        sleep_ms(10);

        chipSelect(SPI0_CS0_PIN);
        ret = spi_write_read_blocking(spi0, txBuffer2, rxBuffer2, 7);
        chipDeselect(SPI0_CS0_PIN);

        sleep_ms(10);
    }
}
