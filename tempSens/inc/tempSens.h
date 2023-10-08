

#include <cstdint>
#include "max31865.h"
#include "spi.h"


class tempSens
{
public:
    /**
     * @brief Returns a pointer to the singleton instance of the tempSens class.
     * 
     * @return tempSens* Pointer to the singleton instance of the tempSens class.
     */
    static tempSens *getInstance(void);

    ~tempSens();
    /**
     * @brief Schedules the temperature sensing task.
     * 
     */
    void startSchedule(void);

    /**
     * @brief Stops the temperature sensing task.
     * 
     */
    void stopSchedule(void);

   
    /**
     * @brief Returns the temperature of the requested sensor.
     * 
     * @param sensorNum Number of the sensor.
     * 
     * @return float Temperature in °C.
     */
    float getTemperature(uint8_t sensorNum);

    /**
     * @brief Gets the temperature readings from the sensor.
     * 
     * @param temperatures An array to store the temperature readings.
     * @param length The length of the temperature readings array.
     */
    void getTemperature(float temperatures[], const uint8_t length);


    void setConfig(max31865Config sensorConfig[], const uint8_t numOfSensors);

private:


    /**
     * @brief A boolean flag indicating whether the temperature sensing schedule is currently running.
     */
    bool scheduleRunning = false;
    bool initialized = false;
    bool configValuesReady =false;
 

    static const uint32_t scheduleCycleTime = 1000; //ms
    uint8_t numOfSensors = 4;

    // singelton, hide all constructs
    tempSens(void);
    tempSens(tempSens const &);
    void operator=(tempSens const &);

    
    typedef void (tempSens::*sensingTaskPtrType)(void *pvParameters);

    static sensingTaskPtrType sensingTaskPtr;

    static void sensingClassTask(void *pvParameters);
    void sensingTask(void);
    void initializeSensors(void);

    
    max31865 **max31865DevicesArrayPtr;
    
    float *temperatureArray;
    max31865::errorCode_t *max31865ErrorCodeArray;
    uint32_t *max31865ErrorCountArray;

};