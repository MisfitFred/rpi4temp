# BBQ Thermometer

Major aim of this project is personal C++ education, therefore some
solutions may be over engineered or not properly implemented. The project 
is a BBQ thermometer with the following features:

* 4 thermocouple inputs
* Web interface for configuration and monitoring 
  (this might be realized with a separate Raspberry Pi)
* Relay outputs for controlling fans
* PID control for temperature regulation (Maybe a PI controller is sufficient?)

## Hardware

The base of this project is a Raspberry Pi Pico. The Pico is a micro controller with a dual core ARM Cortex M0+ processor. To measure the temperature, four MAX31865 evaluation boards are used. Such boards are quite cheap and can be found on AliExpress. The MAX31865 is a thermocouple sensor with SPI interface. The pin-out is quite simple, respective Pico pins can be found in the code. To differentiate the sensors, the CS pin is used.

PT100 sensors shall be used to avoid the need to rework the MAX31865 evaluation boards. They are equipped with the needed reference resistor for PT100.

Till now, no relay card is selected.

## Software

The software is written in C++ and uses the Raspberry Pi Pico SDK. The path to the SDK shall be provided by the environment variable `PICO_SDK_PATH`.

Feature status:

* [x] SPI communication with MAX31865
* [ ] Logging
* [ ] WLAN communication
* [ ] Relay control
* [ ] PID control
* ...
  
## Build

The project can be build with the following commands:

```bash
    mkdir build
    cd build
    cmake ..
    make
```

## Mechanic

A housing shall be designed and printed with a 3D printer to house the respective hardware described above.
