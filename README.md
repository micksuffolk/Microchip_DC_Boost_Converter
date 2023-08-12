# Microchip MPLAB DC Boost Converter

This project contains the design for a DC boost converter to boost a 5VDC supply up to circa 12VDC using a PIC microcontroller to switch an inductor in a boost arrangement using PWM.

- 5VDC Input boosted to 9VDC-12VDC output (using 33% to 73% PWM duty cycle respectively)
- 37.5mA rated current (using the provided circuit schematic & parts)
- Output ripple voltage less than 40mV
- 200kHz PWM frequency used
- 100uH inductor switched in boost arrangement using basic NPN low power transistor (BC108)
- C code written & compiled using Microchip MPLAB X IDE & XC8 compiler for a PIC16F887