#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "max31865.h"
#include "spi.h"

#include "FreeRTOS.h"
#include "task.h"

const uint8_t SPI0_SCK_PIN = 2;
const uint8_t SPI0_TX_PIN = 3;
const uint8_t SPI0_RX_PIN = 4;

const uint8_t SPI0_CS0_PIN = 5;
const uint8_t RDY0_PIN = 6;

const uint8_t SPI0_CS1_PIN = 7;
const uint8_t RDY1_PIN = 8;

const uint8_t SPI0_CS2_PIN = 9;
const uint8_t RDY2_PIN = 10;

const uint8_t SPI0_CS3_PIN = 11;
const uint8_t RDY3_PIN = 12;

void r_readMax31865Register(void)
{
    static spi *mySpiObj = spi::getInstance(0);
    mySpiObj->setMode(SPI_MODE_1);

    static max31865 max31865Devices[4] = {
        max31865(mySpiObj, SPI0_CS0_PIN, RDY0_PIN),
        max31865(mySpiObj, SPI0_CS1_PIN, RDY1_PIN),
        max31865(mySpiObj, SPI0_CS2_PIN, RDY2_PIN),
        max31865(mySpiObj, SPI0_CS3_PIN, RDY3_PIN)};

    max31865::errorCode_t error[4];
    static int errorCount[4]{0, 0, 0, 0};

    static bool initialized = false;
    if (!initialized)
    {

        // static max31865 *myMax31865_dev0 = new max31865(mySpiObj, SPI0_CS0_PIN, RDY0_PIN);

        // check connection for all 4 devices

        for (int i = 0; i < 4; i++)
        {
            error[i] = max31865Devices[i].testConnection();
            if (error[i] != max31865::NO_ERROR)
            {
                errorCount[i]++;
            }
        }

        pt100_optimized50to120 *myPT100 = new pt100_optimized50to120();

        struct max31865Config myConfig = {
            .threeWireMode = false,
            .upperThresholdInPercent = 10,
            .lowerThresholdInPercent = 10,
            .sensorType = myPT100,
            .filter50Hz = true,
            .faultDetectionOn = true};   

        for (int i = 0; i < 4; i++)
        {
            max31865Devices[i].startContinuousConversion(myConfig);
        }
        initialized = true;
    }

    float temperature[4];
    for (int i = 0; i < 4; i++)
    {
        if (max31865Devices[i].isConversionResultReady())
        {           
            error[i] = max31865Devices[i].readTemperature(temperature[i]);
            if (error[i] != max31865::NO_ERROR)
            {
                errorCount[i]++;
            }
        }
    }

#if 0
    static configRegister_t *writeConfigRegister_1 = new configRegister_t();
    writeConfigRegister_1->filter = configRegister_t::filter_t::FILTER_50HZ;

    static configRegister_t *writeConfigRegister_2 = new configRegister_t();
    writeConfigRegister_2->vBias = configRegister_t::vBias_t::ON;
    writeConfigRegister_2->conversionMode = configRegister_t::conversionMode_t::AUTO;
    writeConfigRegister_2->oneShot = configRegister_t::oneShot_t::SHOT_OFF;
    writeConfigRegister_2->wire = configRegister_t::wire_t::WIRE_4;
    writeConfigRegister_2->filter = configRegister_t::filter_t::FILTER_50HZ;
    writeConfigRegister_2->faultDetection = configRegister_t::faultDetection_t::NO_FAULT;

    static configRegister_t *readConfigRegister_1 = new configRegister_t();
    static configRegister_t *readConfigRegister_2 = new configRegister_t();

    myMax31865->writeRegister(*writeConfigRegister_1);
    myMax31865->readRegister(*readConfigRegister_1);
    myMax31865->writeRegister(*writeConfigRegister_2);
    myMax31865->readRegister(*readConfigRegister_2);
#endif
}

/**
 * @brief Task function 10ms
 *
 */
void task_10ms(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(10);
    xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/**
 * @brief Task function 100ms
 */
void task_100ms(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(100);
    xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/**
 * @brief Task function 1000ms
 */
void task_1000ms(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);
    xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        r_readMax31865Register();

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/**
 * @brief Create all needed tasks and start the scheduler
 *
 * - 10ms Task to toggle pin 25
 * - 100ms Task to toggle pin 26
 * - 1000ms Task to read the Max31685 temperature
 *
 */
void startTasks(void)
{
    // creates the 10ms task
    xTaskCreate(task_10ms, "10msTask", 512, NULL, 1, NULL);
    xTaskCreate(task_100ms, "100msTask", 512, NULL, 1, NULL);
    xTaskCreate(task_1000ms, "1000msTask", 512, NULL, 1, NULL);

    // Start scheduler
    vTaskStartScheduler();
}

void initCsPin(uint8_t pin, bool init)
{
    // set pin mux for SPI
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, init); // cs is active low, initialize as high
}

void initRdyPin(uint8_t pin)
{
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
}

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

/*
 *  GPIO   | F1
 *  -------|----------
 *  2      | SPI0_SCK_PIN
 *  3      | SPI0_TX_PIN
 *  4      | RDY0_PIN
 *  5      | SPI0_RX_PIN
 *  6      | SPI0_CS0_PIN
 */

// main function
int main(int argc __attribute__((unused)), char const *argv[] __attribute__((unused)))
{
    stdio_init_all();
    init_PinMux();
    startTasks();

    while (1)
    {
        sleep_ms(1000);
    }
}
