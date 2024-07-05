# Repost Declaration

I would like to declare that this EXAMPLE is a repost. 
Source: https://esp32.com/viewtopic.php?f=25&t=35035&p=130550&hilit=uart_dma#p130550.

# Technical Requirements:

ESPIDF PLUGIN v1.4.0 (for Python 3.8)
ESPIDF V5.4 (git checkout 2e68e510a5163c106ea04182b6ffe3063630b6c1)
Not applicable to ESP32 and ESP32-S2 without GDMA

# Suuported Targets:
| ESP32-S2 | ESP32-S3 | ESP32-C3 |


# UART DMA (UHCI) Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows you how the use the uart with dma, also named UHCI driver.


## How to use example

### Hardware Required

The example can be used with any ESP32 development board connected to a computer with a USB cable.

1. Any ESP development boart. Including ESP32, ESP32-S2, ESP32-C3.

2. To connect UART to PC, another board such as ESP_Test Board or FT232 USB UART board is usually needed.

3. Two USB cables, one for UART0 for flashing and monitoring, another for other UARTs for testing reading and writing data.

### Configure the project

```
idf.py menuconfig
```

* Set serial port under Serial Flasher Options.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```
or
```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Sendind `00 01 02 03 04 05` will generate the following output:
```
...
I (274) cpu_start: Starting scheduler.
I (279) uhci-example: UART-DMA(UHCI) example start, tx io:[8], rx io:[9], baud rate:[115200]
I (319) uhci-example: uhci write buffer done!
I (4859) uhci-example: bytes correct, bingo~!
I (4859) uhci-example: bytes correct, bingo~!
I (4859) uhci-example: bytes correct, bingo~!
I (4869) uhci-example: bytes correct, bingo~!
I (4869) uhci-example: bytes correct, bingo~!
I (4879) uhci-example: bytes correct, bingo~!
```
