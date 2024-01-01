#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "max31865.h"
#include "spi.h"
#include "tempSens.h"

#include "udp_beacon.h"
#include "wifi_manager.h"
#include "tempProvider.h"

#include "FreeRTOS.h"
#include "task.h"

#include "pinMux.h"

#include "pico/cyw43_arch.h"
void main_init_task(void *pvParameters);
int main(int argc __attribute__((unused)), char const *argv[] __attribute__((unused)))
{

    stdio_init_all();
    init_PinMux();

    // task handler
    TaskHandle_t task;

    if (pdPASS == xTaskCreate(main_init_task, "main init task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2UL, &task))
    {
        printf("udp beacon task created\n");
    }
    else
    {
        printf("udp beacon task creation failed\n");
    }

    // create a PT100 sensor object able to handle several max devices,  optimized for 50 to 120 degree celsius
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

    /*
        tempSens *myTempSens = tempSens::getInstance();
        myTempSens->setConfig(myConfig, 4);
        myTempSens->startSchedule();
    */

    // freeRTOS scheduler start
    vTaskStartScheduler();

    while (1)
    {
        sleep_ms(1000);
    }
}

void main_init_task(void *pvParameters)
{
    /* initialize cyw43 and prevent other task be called in between */

    cyw43_arch_init();

    udp_beacon_init();
    wifi_manager_init();
    tempProvider_start();

    udp_beacon_start();
    wifi_manager_start();

    vTaskDelete(NULL);
}
