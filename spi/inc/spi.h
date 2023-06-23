#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <string>
#include <cstring>
#include <stdexcept>
#include "log.h"
#include "gpio.h"

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

/**
 * @brief SPI class to handle SPI communication
 *
 */
class spi
{
public:
    spi(std::string dev, unsigned long int mode, uint32_t speed);
    spi();
    ~spi();

    void openDevice(void);
    void closeDevice(void);
    void setMode(void);
    void setSpeed(void);
    void transmit(spiData *txData, spiData *rxData, uint32_t csPin);

protected:
    void fillSpiTransferStruct(void);

    struct spi_ioc_transfer spiTransfer;

    // SPI parameters
    unsigned long int spiMode = SPI_MODE_1;
    uint32_t spiSpeed = 10000;
    std::string spiDevice = "/dev/spidev0.0";

private:
    int deviceHandler = 0;
    Log::Logger *log;
    gpio *gpioHandler;

    bool spi_transmit_setSpiTransferTxBuffer(spiData *txData);
    bool spi_transmit_setSpiTransferRxBuffer(spiData *rxData, spiData *txData);
};
