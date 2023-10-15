#include "gmock/gmock.h"  // Brings in gMock.
#include "spi.h"

class mockSpi : public spi {
 public:
    MOCK_METHOD(void, openDevice, ());
    MOCK_METHOD(void, closeDevice, (), ());
    MOCK_METHOD(void, setMode, (), ());
    MOCK_METHOD(void, setSpeed, (), ());
    MOCK_METHOD(void, transmit, (spiData *txData, spiData *rxData, int length), ());
};