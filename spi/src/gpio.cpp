#include "gpio.h"
#include <pigpio.h>
#include <pigpiod_if2.h>


gpio::gpio()
{
    log = new Log::Logger(&std::cout, Log::Logger::DEBUG);
}
gpio::~gpio()
{
    delete log;
}







    gpio_pigpio::gpio_pigpio() : gpio()
    {
        if (gpioInitialise() < 0)
        {
            log->fatal() << "Could not initialize pigpio...";
            exit(EXIT_FAILURE);
        }
    }
    gpio_pigpio::~gpio_pigpio()
    {
        gpioTerminate();
    }

    void gpio_pigpio::write(int pin, int value)
    {

        if (gpioWrite(pin, value) != 0)
        {
            log->fatal() << "Could not set GPIO value...";
            exit(EXIT_FAILURE);
        }
    }
    int gpio_pigpio::read(int pin)
    {
        int ret = gpioRead(pin);
        if (ret < 0)
        {
            log->fatal() << "Could not read GPIO value...";
            exit(EXIT_FAILURE);
        }

        return ret;
    };
    void gpio_pigpio::setMode(int pin, int mode)
    {
        if (gpioSetMode(pin, mode) != 0)
        {
            log->fatal() << "Could not set GPIO mode...";
            exit(EXIT_FAILURE);
        }
    }

    gpio_rgpio::gpio_rgpio() : gpio()
    {
        sbc = pigpio_start(NULL, NULL);
        if (sbc < 0)
        {
            log->fatal() << "Could not initialize rgpio...";
            exit(EXIT_FAILURE);
        }
    }
    gpio_rgpio::~gpio_rgpio()
    {
        pigpio_stop(sbc);
    }

    void write(int pin, int value)
    {

        if (gpio_write(sbc, pin, value) != 0)
        {
            log->fatal() << "Could not set GPIO value...";
            exit(EXIT_FAILURE);
        }
    }
    int gpio_rgpio::read(int pin)
    {
        int ret = gpio_read(sbc, pin);
        if (ret < 0)
        {
            log->fatal() << "Could not read GPIO value...";
            exit(EXIT_FAILURE);
        }

        return ret;
    };
    void gpio_rgpio::setMode(int pin, int mode)
    {
        if (set_mode(sbc, pin, mode) != 0)
        {
            log->fatal() << "Could not set GPIO mode...";
            exit(EXIT_FAILURE);
        }
    }
