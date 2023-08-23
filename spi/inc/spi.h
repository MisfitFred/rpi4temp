#pragma once

#include <string>
#include <cstring>
//#include <stdexcept>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// #include <fcntl.h>
// #include <unistd.h>

// #include "log.h"
// #include "gpio.h"

class spiData
{
public:
    /**
     * @brief Construct a new spi Data object
     *
     * @param size
     */
    spiData(uint32_t size);

    /**
     * @brief Destroy the spi Data object
     */
    ~spiData();

    /**
     * @brief Get the Buffer P object
     *
     * @return uint8_t*
     */
    uint8_t *getBufferP(void);

    /**
     * @brief Return the size of the internal buffer
     */
    uint32_t getBufferSize(void);
    /**
     * @brief Get the Buffer Address as integer
     *
     * @return unsigned long
     */
    unsigned long getBufferAdr(void);

    /**
     * @brief behave like an array
     *
     * @param index array index
     * @return uint8_t reference
     */
    uint8_t &operator[](int index);

    /**
     * @brief behave like an array
     *
     * @param index array index
     * @return const uint8_t reference
     */
    constexpr const uint8_t &operator[](int index) const noexcept;

    /**
     * @brief Convert the SPI data to a string
     *
     * @return std::string
     */
    std::string toString(void);

    /**
     * @brief Print the SPI data to stdout
     */
    void print(void);

private:
    uint8_t *buffer = nullptr;
    unsigned int size = 0;
};

enum spiMode_e
{
    SPI_MODE_0 = 0,
    SPI_MODE_1 = 1,
    SPI_MODE_2 = 2,
    SPI_MODE_3 = 3
};
/**
 * @brief SPI class to handle SPI communication
 *
 */
class spi
{
public:
    static spi *getInstance(uint8_t spiDev);
    void setMode(spiMode_e mode);
    void setSpeed(uint32_t speed);
    ~spi();

    void transmit(spiData *txData, spiData *rxData, uint32_t csPin);

protected:
    // SPI parameters
    uint32_t spiSpeed = 0x20000;
    uint32_t spiSpeedReal = 0;
    uint8_t spiDev = 0u;
    spi_cpol_t cpol = SPI_CPOL_0;
    spi_cpha_t cpha = SPI_CPHA_1;

private:
    spi(int instNum);
    static spi *instance[2];

    int instanceNumber;
    spi_inst_t *spiHwInstance = nullptr;

    bool csActiveValue = false;
    bool csInactiveValue = true;

    void setSpiHwInstancePtr(uint8_t spiDev);

    bool spi_transmit_setSpiTransferTxBuffer(spiData *txData);
    bool spi_transmit_setSpiTransferRxBuffer(spiData *rxData, spiData *txData);

    void chipSelect(uint8_t csPin);
    void chipDeselect(uint8_t csPin);
};
