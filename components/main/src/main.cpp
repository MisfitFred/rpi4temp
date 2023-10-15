#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "max31865.h"
#include "spi.h"
#include "tempSens.h"

#include "FreeRTOS.h"
#include "task.h"

#include "pinMux.h"


int main(int argc __attribute__((unused)), char const *argv[] __attribute__((unused)))
{
    stdio_init_all();
    init_PinMux();
   
    pt100_optimized50to120 *myPT100 = new pt100_optimized50to120();
    spi *mySpiObj = spi::getInstance(0);
    struct max31865Config myConfig[4] = {
        {.spiObj = mySpiObj,
         .csPin = SPI0_CS0_PIN,
         .rdyPin = RDY0_PIN,
         .threeWireMode = false,
         .upperThresholdInPercent = 0,
         .lowerThresholdInPercent = 0,
         .sensorType = myPT100,
         .filter50Hz = true,
         .faultDetectionOn = false},
        {.spiObj = mySpiObj,
         .csPin = SPI0_CS1_PIN,
         .rdyPin = RDY1_PIN,
         .threeWireMode = false,
         .upperThresholdInPercent = 0,
         .lowerThresholdInPercent = 0,
         .sensorType = myPT100,
         .filter50Hz = true,
         .faultDetectionOn = false},
        {.spiObj = mySpiObj,
         .csPin = SPI0_CS2_PIN,
         .rdyPin = RDY2_PIN,
         .threeWireMode = false,
         .upperThresholdInPercent = 0,
         .lowerThresholdInPercent = 0,
         .sensorType = myPT100,
         .filter50Hz = true,
         .faultDetectionOn = false},
        {.spiObj = mySpiObj,
         .csPin = SPI0_CS3_PIN,
         .rdyPin = RDY3_PIN,
         .threeWireMode = false,
         .upperThresholdInPercent = 0,
         .lowerThresholdInPercent = 0,
         .sensorType = myPT100,
         .filter50Hz = true,
         .faultDetectionOn = false}};


    tempSens *myTempSens = tempSens::getInstance();
    myTempSens->setConfig(myConfig, 4);
    myTempSens->startSchedule();


    vTaskStartScheduler();

    while (1)
    {
        sleep_ms(1000);
    }
}
