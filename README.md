# BITalino Firmware

The BITalino firmware is available in source code and in a prebuilt image ready to be programmed in a device.

## Programming the firmware

To program the firmware on the BITalino device, you will need an AVR ISP programmer like the Atmel AVRISP mkII. The 6-pin ISP interface shall be connected to BITalino using the following pin mapping:

ISP pin | BITalino pin
--------|-------------
  MISO  |    O3
  VCC   |    AVCC
  SCK   |    O4
  MOSI  |    O2
  RESET |    RST
  GND   |    AGND

The prebuilt image is the [main.hex](https://raw.githubusercontent.com/BITalinoWorld/firmware-bitalino/master/prebuilt/main.hex) file provided.

### Programming on Windows

- Atmel Studio must be installed;
- connect the ISP programmer to your computer and wait for the its drivers to be installed (first time only);
- connect the programmer to the BITalino device;
- on Atmel Studio, open the Device Programming dialog;
- select the programmer in the Tool field;
- select the ATmega328P in the Device field;
- select ISP in the Interface field;
- click on the Apply button;
- on Flash section of the Memories tab, select the [main.hex](prebuilt/main.hex) file;
- click on the Program button of the Flash section and check if the operation succeeded;
- on the Fuses tab, enter the following values to the fuse registers: Extended = 0xFD, High = 0xDF, Low = 0xFF;
- click on the Program button and check if the operation succeeded.

### Programming on Linux and Mac OS X

- package `avrdude` must be installed (in Linux, you can enter the command `sudo apt-get install avrdude` and in Mac OS X you can use `fink`, entering the command `sudo fink install avrdude`);
- connect the ISP programmer to your computer;
- connect the programmer to the BITalino device;
- on the same directory as the provided [main.hex](prebuilt/main.hex) file, enter the command `avrdude -P usb -p m328p -c avrispmkii -B 5 -U flash:w:main.hex -U lfuse:w:0xFF:m -U hfuse:w:0xDF:m -U efuse:w:0x05:m` (change the -c option if your programmer is not an AVRISP mkII) and check if the operation succeeded.


## Compiling the firmware

You may want to compile the firmware from the source code instead of using the provided firmware image. The firmware source code is written in C. You can use and modify the firmware source code under the terms of the firmware licence (GPL).

### Compiling on Windows

- Atmel Studio must be installed;
- on Atmel Studio, create a GCC C Executable Project;
- select the ATmega328P device for the new project;
- remove the .c file automatically created for the new project;
- add the firmware source files to the project;
- on the Build tab of the project properties, select "Use External Makefile" and select the firmware makefile;
- build the solution;
- to program the device, follow the instructions on the Programming on Windows section above, but using the main.hex file created on the `build` directory instead of the provided firmware image file.

### Compiling on Linux

- packages `make`, `gcc-avr`, `avr-libc`, `binutils-avr` and `avrdude` must be installed (you can use `apt-get` or `synaptic` to install them);
- on the same directory as the source files, enter the command `make` to build the firmware;
- on the same directory, enter the command `make send` to program the BITalino device using `avrdude`.

### Compiling on Mac OS X

- packages `avr-gcc`, `avr-libc`, `avr-binutils` and `avrdude` must be installed (you can use `fink` to install them);
- on the same directory as the source files, enter the command `make` to build the firmware;
- on the same directory, enter the command `make send` to program the BITalino device using `avrdude`.
