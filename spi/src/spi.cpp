#include "spi.h"

spi::spi(std::string dev, unsigned long int mode, uint32_t speed) : spi()
{
    this->spiDevice = dev;
    this->spiMode = mode;
    this->spiSpeed = speed;
    fillSpiTransferStruct();
}

spi::spi()
{
    log = new Log::Logger(&std::cout, Log::Logger::DEBUG);
    fillSpiTransferStruct();
    gpioHandler = new gpio_rgpio();
}

spi::~spi()
{
    delete log;
    delete gpioHandler;
}

int add(int a, int b);

void spi::fillSpiTransferStruct(void)
{
    spiTransfer.speed_hz = spiSpeed;
    spiTransfer.bits_per_word = 8;
    spiTransfer.delay_usecs = 0;
    spiTransfer.cs_change = 0;
    spiTransfer.word_delay_usecs = 0;
    spiTransfer.tx_nbits = 0;
    spiTransfer.rx_nbits = 0;
    spiTransfer.pad = 0;
}

void spi::openDevice(void)
{
    deviceHandler = open(this->spiDevice.c_str(), O_RDWR);
    if (deviceHandler < 0)
    {
        log->fatal() << "Could not open the SPI device..." << deviceHandler;
        exit(EXIT_FAILURE);
    }

    gpioHandler->setMode(8, PI_OUTPUT);
    gpioHandler->write(8, 1);
}

void spi::closeDevice(void)
{
    close(deviceHandler);
}

void spi::setMode(void)
{
    unsigned long int mode;
    int ret = ioctl(deviceHandler, SPI_IOC_RD_MODE, &mode);
    if (ret != 0)
    {
        log->fatal() << "Could not read SPIMode (RD)...ioctl fail";
        exit(EXIT_FAILURE);
    }
    log->fatal() << "mode: " << std::hex << mode;
    mode |= spiMode;
    log->fatal() << "mode: " << std::hex << mode;
    ret = ioctl(deviceHandler, SPI_IOC_WR_MODE, &mode);
    if (ret != 0)
    {
        log->fatal() << "Could not set SPIMode (WR)...ioctl fail";
        exit(EXIT_FAILURE);
    }
}

void spi::setSpeed(void)
{
    uint32_t speed;
    int ret = ioctl(deviceHandler, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret != 0)
    {
        log->fatal() << "Could not read SPISpeed (RD)...ioctl fail";
        exit(EXIT_FAILURE);
    }

    speed = 10000;
    ret = ioctl(deviceHandler, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret != 0)
    {
        log->fatal() << "Could not set SPISpeed (WR)...ioctl fail";
        exit(EXIT_FAILURE);
    }
}

bool spi::spi_transmit_setSpiTransferTxBuffer(spiData *txData)
{
    if (txData == nullptr)
    {
        log->fatal() << "txData must be set";
        return false;
    }
    else
    {
        spiTransfer.tx_buf = txData->getBufferAdr();
        return true;
    }
}

bool spi::spi_transmit_setSpiTransferRxBuffer(spiData *rxData, spiData *txData)
{
    bool ret = false;
    // checking rxData

    if (txData->getBufferSize() != rxData->getBufferSize())
    {
        log->fatal() << "tx and rx buffer must be the same size";
        ret = false;
    }
    else
    {
        spiTransfer.rx_buf = rxData->getBufferAdr();
        ret = true;
    }

    return ret;
}

void spi::transmit(spiData *txData, spiData *rxData, uint32_t csPin)
{
    spiData *tempRxBuffer = nullptr;

    spi_transmit_setSpiTransferTxBuffer(txData);

    if (rxData == nullptr)
    {
        unsigned int size = txData->getBufferSize();
        log->debug() << "no rx buffer given, create temp buffer with size: " << size << " bytes";
        tempRxBuffer = new spiData(size);
        spi_transmit_setSpiTransferRxBuffer(tempRxBuffer, txData);
    }
    else
    {
        spi_transmit_setSpiTransferRxBuffer(rxData, txData);
    }

    spiTransfer.len = txData->getBufferSize();

    gpioHandler->write(csPin, 0);

    int ret = ioctl(deviceHandler, SPI_IOC_MESSAGE(1), &spiTransfer);
    if (ret != 0)
    {
        log->debug() << "send SPI message... got: " << ret << " bytes";
    }

    gpioHandler->write(csPin, 1);

    log->debug() << txData->toString();

    if (rxData != nullptr)
        log->debug() << rxData->toString();
    else
    {
        log->debug() << tempRxBuffer->toString();
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
    if (index >= this->size)
        throw std::out_of_range("index out of range");
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

/**
 * @brief Print the SPI data to stdout
 */
void spiData::print(void)
{
    std::cout << this->toString() << std::endl;
}
