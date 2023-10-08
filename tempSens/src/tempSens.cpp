

#include "FreeRTOS.h"
#include "task.h"

#include "tempSens.h"

#include "pinMux.h"

/**
 * @brief Constructor for the tempSens class.
 *
 */
void tempSens::setConfig(max31865Config sensorConfig[], const uint8_t numOfSensors)
{
    this->numOfSensors = numOfSensors;
    max31865DevicesArrayPtr = new max31865 *[numOfSensors]
    { nullptr };
    max31865ErrorCodeArray = new max31865::errorCode_t[numOfSensors]{max31865::NO_ERROR};
    max31865ErrorCountArray = new uint32_t[numOfSensors]{0};
    temperatureArray = new float[numOfSensors]{0.0f};

    for (int i = 0; i < numOfSensors; i++)
    {
        this->max31865DevicesArrayPtr[i] = new max31865(&sensorConfig[i]);
    }

    this->scheduleRunning = false;
    this->initialized = false;
    this->configValuesReady = true;
}

tempSens::tempSens()
{
    // do nothing
}

/**
 * @brief Destructor for the tempSens class.
 *
 */
tempSens::~tempSens()
{

    for (int i = 0; i < numOfSensors; i++)
    {
        delete max31865DevicesArrayPtr[i];
    }
    delete[] max31865DevicesArrayPtr;
    delete[] max31865ErrorCodeArray;
    delete[] max31865ErrorCountArray;
    delete[] temperatureArray;
}

/**
 * @brief Gets the instance of the temperature sensor class.
 *
 * @return tempSens* Pointer to the temperature sensor class.
 */
tempSens *tempSens::getInstance(void)
{

    static int numOfInstances = 0;
    static tempSens *tempSensInstance = nullptr;

    if (tempSensInstance == nullptr)
    {
        tempSensInstance = new tempSens();
    }
    return tempSensInstance;
}

/**
 * Initializes the temperature sensors.
 */
void tempSens::initializeSensors(void)
{

    if (!initialized && configValuesReady)
    {
        // check connection for all 4 devices
        for (int i = 0; i < 4; i++)
        {
            max31865ErrorCodeArray[i] = max31865DevicesArrayPtr[i]->testConnection();
            if (max31865ErrorCodeArray[i] != max31865::NO_ERROR)
            {
                max31865ErrorCountArray[i]++;
            }

            max31865ErrorCodeArray[i] = max31865DevicesArrayPtr[i]->startContinuousConversion();
            if (max31865ErrorCodeArray[i] != max31865::NO_ERROR)
            {
                max31865ErrorCountArray[i]++;
            }
        }

        initialized = true;
    }
}

/**
 * @brief   Start to schedule the temperature sensing task.
 *
 * @see stopSchedule()
 */
void tempSens::startSchedule(void)
{

    if (!scheduleRunning && configValuesReady)
    {
        xTaskCreate(sensingClassTask, "sensingTask", 1000, NULL, 1, NULL);
        scheduleRunning = true;
    }
}

/**
 * @brief  Stop to schedule the temperature sensing task
 *
 */
void tempSens::stopSchedule(void)
{

    if (scheduleRunning)
    {
        vTaskDelete(NULL);
        scheduleRunning = false;
    }
}

/**
 * @brief Task implementation of the whole class to trigger the
 * temperature sensing of all instances.
 *
 * @remarks Only one instance is foreseen yet, so there might
 * be a more elegant way to do this. But to play around with
 * the FreeRTOS, this is a good start.
 *
 * @param pvParameters
 */
void tempSens::sensingClassTask(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(scheduleCycleTime);
    xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        tempSens::getInstance()->sensingTask();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void tempSens::sensingTask(void)
{

    static float temp0 = 0.0f;
    static float temp1 = 0.0f;
    static float temp2 = 0.0f;
    static float temp3 = 0.0f;

    if (!initialized)
    {
        initializeSensors();
    }
    for (int i = 0; i < this->numOfSensors; i++)
    {
         max31865ErrorCodeArray[i] =
            max31865DevicesArrayPtr[i]->readTemperature(temperatureArray[i]);
             if (max31865ErrorCodeArray[i] != max31865::NO_ERROR)
        {
            max31865ErrorCountArray[i]++;
        }
    }

    temp0 = temperatureArray[0];
    temp1 = temperatureArray[1];
    temp2 = temperatureArray[2];
    temp3 = temperatureArray[3];
}

/**
 * @brief Returns the temperature of the requested sensor.
 *
 * @param sensorNum Number of the sensor.
 *
 * @return float Temperature in °C.
 */
float tempSens::getTemperature(uint8_t sensorNum)
{
    if (sensorNum < numOfSensors)
    {
        return temperatureArray[sensorNum];
    }
    else
    {
        return 0.0f;
    }
}

/**
 * @brief Gets the temperature readings from the sensor.
 *
 * @param temperatures An array to store the temperature readings.
 * @param length The length of the temperature readings array.
 */
void tempSens::getTemperature(float temperatures[], const uint8_t length)
{
    if (length <= numOfSensors)
    {
        for (int i = 0; i < length; i++)
        {
            temperatures[i] = temperatureArray[i];
        }
    }
}