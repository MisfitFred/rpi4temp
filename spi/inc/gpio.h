
#include <pigpio.h>
#include <pigpiod_if2.h>
#include "log.h"
class gpio
{

public:
    enum pinLevel
    {
        LOW = 0,
        HIGH = 1
    };

    enum pinMode
    {
        GPIO_INPUT = 0,
        GPIO_OUTPUT = 1,
        GPIO_ALT0 = 4,
        GPIO_ALT1 = 5,
        GPIO_ALT2 = 6,
        GPIO_ALT3 = 7,
        GPIO_ALT4 = 3,
        GPIO_ALT5 = 2
    };

    gpio();
    virtual ~gpio();
    virtual void write(int pin, int value) = 0;
    virtual int read(int pin) = 0;
    virtual void setMode(int pin, int mode) = 0;

protected:
    Log::Logger *log;
};

class gpio_pigpio : public gpio
{
public:
    gpio_pigpio();
    ~gpio_pigpio();

    void write(int pin, int value) override;
    int read(int pin) override;
    void setMode(int pin, int mode) override;
};

class gpio_rgpio : public gpio
{
public:
    gpio_rgpio();
    ~gpio_rgpio() ;
    void write(int pin, int value) override;
    int read(int pin) override;
    void setMode(int pin, int mode) override;

private:
    int sbc = -1;
};