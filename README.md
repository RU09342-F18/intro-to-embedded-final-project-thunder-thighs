# Final Project: Ultrasonic Parking Sensor

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
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// Disable the watchdog timer
	
	// Function Declarations
	Timer_Setup();
	LED_Setup();
	UART_Setup();

	__bis_SR_register(LPM0_bits + GIE); // Enter Low Power Mode once interrupt is detected & Global Interrupt Enable
    __enable_interrupt();   // Enable interrupt algorithm

    for(;;){}; // Infinite for loop
}
```
