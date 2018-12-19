# Final Project: Ultrasonic Parking Sensor
The overall operation of this design revolves around detecting distance using an ultrasonic sensor. The main components responsible for the device's operation are the MSP4305529 microcontroller and the HC-SR04 Ultrasonic sensor. The HC-SR04 uses ultrasonic waves to detect distance by transmitting a sound wave from one terminal and receiving the reflected wave back in the other. This allows for a fairly accurate distance reading for objects located in front of the sensor for up to 2 meters. Using the output of this device, the ADC on the microcontroller is able to convert the reading to a distance measurement (cm). Based on the distance detected, the five indicator LEDs illuminate accordingly so that the user can gauge the distance.

There are five indicator LEDs that are used to indicate how close an object is with relation to the sensor. Each LED is assigned a distance value. Once the object in front of the sensor is equal to or less than the value, the respective LED lights up. UART was also utilized for this project. The UART transmit buffer displayed the distance readings in real time, which adjusted based on the distance detected. The receive buffer was also utilized in setting new distance values. For example, if the user wanted to change the distance thresholds for each LED, this could be done by sending a package of bytes through UART.

# Authors
Nick Scamardi, Eric Schroeder & Nick Setaro are the three contributors to this Project.

# Dependencies
The library dependencies for the code behind the sensor include the standard `<msp430.h>` library, `<stdint.h>` and `<intrinsics.h>`.

# Components
The main components used to build this milestone include:
* TI MSP430F5529 Microcontroller
* HC-SR04 Ultrasonic Sensor
* 5 LEDs (White, Blue, Green, Yellow, Red)
* Code Composer Studio 8.1.0


# Functionality of main.c

```c

```
