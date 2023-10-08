
#include "spi.h"
//#include <iostream>
//#include <ostream>
#include <sstream>

//@Bug:  singelton implementation is not correct

spi *spi::instance[2] = {nullptr, nullptr};

spi::spi(int instNum)
{
    this->instanceNumber = instNum;
    setSpiHwInstancePtr(instNum);
    this->spiSpeedReal = spi_init(this->spiHwInstance, this->spiSpeed);
	
	}

spi::~spi()
{
    spi_deinit(this->spiHwInstance);
}

int add(int a, int b);

/**
 * @brief Get the spi Instance object
 *
 * @param spiInstanceNumber
 */

spi *spi::getInstance(uint8_t spiInstanceNumber)
{
    // not more than two instances are allowed since RP2040 has only two SPI interfaces
    if (spiInstanceNumber > 1)
    {
        return nullptr;
    }

    // create new instance if not already done
    if (instance[spiInstanceNumber] == nullptr)
    {
        instance[spiInstanceNumber] = new spi(spiInstanceNumber);
    }

    return instance[spiInstanceNumber];
}

void spi::setSpiHwInstancePtr(uint8_t spiInst)
{
    switch (spiInst)
    {
    case 0:
        this->spiHwInstance = spi0;
        break;
    case 1:
        this->spiHwInstance = spi1;
        break;
    default:
        this->spiHwInstance = nullptr;
    }
}

void spi::setMode(spiMode_e mode)
{
    switch (mode)
    {
    case SPI_MODE_0:
        this->cpol = SPI_CPOL_0;
        this->cpha = SPI_CPHA_0;
        break;
    case SPI_MODE_1:
        this->cpol = SPI_CPOL_0;
        this->cpha = SPI_CPHA_1;
        break;
    case SPI_MODE_2:
        this->cpol = SPI_CPOL_1;
        this->cpha = SPI_CPHA_0;
        break;
    case SPI_MODE_3:
        this->cpol = SPI_CPOL_1;
        this->cpha = SPI_CPHA_1;
        break;
    }

    spi_set_format(this->spiHwInstance, 8, this->cpol, this->cpha, SPI_MSB_FIRST);
}

void spi::setSpeed(uint32_t speed)
{
    this->spiSpeed = speed;
    spi_set_baudrate(this->spiHwInstance, this->spiSpeed);
}

void spi::chipSelect(uint8_t csPin)
{
    asm volatile("nop \n nop \n nop");
    gpio_put(csPin, this->csActiveValue);
    asm volatile("nop \n nop \n nop");
}

void spi::chipDeselect(uint8_t csPin)
{
    asm volatile("nop \n nop \n nop");
    gpio_put(csPin, this->csInactiveValue);
    asm volatile("nop \n nop \n nop");
}

/**
 * @brief Transmit data via SPI
 *
 * @param txData transmit data buffer
 * @param rxData receiver data buffer, can be nullptr, then an internal buffer is created but the data is thrown away
 */
void spi::transmit(spiData *txData, spiData *rxData, uint32_t csPin)
{
    spiData *tempRxBuffer = nullptr;

    if (rxData == nullptr)
    {
        unsigned int size = txData->getBufferSize();
        // log->debug() << "no rx buffer given, create temp buffer with size: " << size << " bytes";
        tempRxBuffer = new spiData(size);
    }
    else
    {
        if (txData->getBufferSize() != rxData->getBufferSize())
        {
            // log->fatal() << "tx and rx buffer must be the same size";
        }
    }

    chipSelect(csPin);
    uint8_t *txBuffer = txData->getBufferP();
    uint8_t *rxBuffer = rxData->getBufferP();
    uint32_t size = txData->getBufferSize();
    int ret = spi_write_read_blocking(this->spiHwInstance, txBuffer, rxBuffer, size);

    if (ret != 0)
    {
        // log->debug() << "send SPI message... got: " << ret << " bytes";
    }
    chipDeselect(csPin);

    if (rxData == nullptr)
    {
        // log->debug() << tempRxBuffer->toString();
        delete tempRxBuffer;
    }
}

/**
 * @brief Construct a new spi Data object
 *
 * @param size
 */
spiData::spiData(uint32_t s)
{
    this->size = s;
    this->buffer = new uint8_t[s];
    memset(this->buffer, 0, s);
}

/**
 * @brief Destroy the spi Data object
 */
spiData::~spiData()
{
    delete[] buffer;
}

/**
 * @brief Get the Buffer P object
 *
 * @return uint8_t*
 */
uint8_t *spiData::getBufferP(void)
{
    return this->buffer;
}

/**
 * @brief Return the size of the internal buffer
 */
uint32_t spiData::getBufferSize(void)
{
    return this->size;
}
/**
 * @brief Get the Buffer Address as integer
 *
 * @return unsigned long
 */
unsigned long spiData::getBufferAdr(void)
{
    return (unsigned long)this->buffer;
}

/**
 * @brief behave like an array
 *
 * @param index array index
 * @return uint8_t
 */
uint8_t &spiData::operator[](int index)
{
    // std::cout << "this->buffer: " << (unsigned int)this->buffer << std::endl;
    // std::cout << "#############";
    // std::cout << "this->buffer[" << index << "] = " << ((unsigned int)&(this->buffer[index])) << "#" << std::endl;
    return (buffer[index]);
}
/*
std:array<uint8_t, 8> spiData::getBuffer(void)
{
    return this->buffer;
}*/
/**
 * @brief behave like an array
 *
 * @param index array index
 * @return const uint8_t
 */
constexpr const uint8_t &spiData::operator[](int index) const noexcept
{
    //if (index >= this->size)
        //throw std::out_of_range("index out of range");
    return (buffer[index]);
}



/**
 * @brief Convert the SPI data to a string
 *
 * @return std::string
 */
std::string spiData::toString(void)
{
    std::stringstream ss;
    std::string str;

    for (unsigned int i = 0; i < this->size; i++)
    {
        ss << std::hex << (int)this->buffer[i];
        ss << " ";
    }
    str = ss.str();
    return str;
}


#if 0
/**
 * @brief Print the SPI data to stdout
 */
void spiData::print(void)
{
    std::cout << this->toString() << std::endl;
}
#endif
