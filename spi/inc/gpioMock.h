#include "gmock/gmock.h"  // Brings in gMock.
#include "gpio.h"

class mockGpio : public gpio {
 public:
 MOCK_METHOD(void, write, (int pin, int value), ());
 MOCK_METHOD(int, read, (int pin), ());
 MOCK_METHOD(void, setMode, (int pin, int mode), ());
};