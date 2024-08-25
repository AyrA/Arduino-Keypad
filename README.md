# Arduino Keypad PIN Input

Asks the user for a pin (1-10 digits) using an LCD screen and a matrix keypad.

## Features

- Pin inpout via keypad
- Displays information to the user on an LCD
- LCD uses I2C bus to reduce the number of pins necessary
- Resets automatically if it gets stuck
- Sends pin via serial connection

## Hardware

This project needs 3 basic components: CPU board, display, and keypad.

### Requirements

- Arduino (nano or uno), or a compatible device with at least 7 digital and 2 analog channels.
- Character LCD with 20 columns and 4 rows (optional, you can of course rewrite the code to fit your display)
- Matrix keypad (see data folder for an image)

The example image in the data folder shows the device.
Note that for an arduino, the programmer (top right) is not necessary as it can be programmed via USB

#### Additional Hardware

- Cables, or a breaboard, or prototyping board to connect the components
- Pin headers for the keypad and a soldering iron to solder the pin header to it

### Purchasing Hardware

- Arduino: https://www.aliexpress.com/item/1005005069791121.html
- LCD: https://www.aliexpress.com/item/1005005973980073.html
- Keypad: https://www.aliexpress.com/item/1005006247498377.html

*The owner of this repository does not endorses or gets a sale commission for any of the given links.*
*These links are merely a help to get the correct product type.*

In case the links expire and you need to find alternative sources, see blow:

- Arduino: Any alternative Atmel based board should work that is nano or uno compatible
- LCD: Any character LCD based on the HD44780 chip with an I2C module will work
- Keypad: Any 3x4 matrix keypad will work, although you likely need to adjust the pin settings if you get a different keypad because the wiring is not the same across different keypad types.

## Assembly

Assembly is fairly straightforward and merely requires connecting the components

### LCD

The I2C pins of the LCD are connected as follows:

| I2C | Arduino |
|-----|---------|
| VCC | 5V      |
| GND | GND     |
| SDA | A4      |
| SCL | A5      |

### Keypad

The keypad has 9 pins. Looking at the front, we can number them from 1 to 9 from left to right.
The first and last pin are not connected

| Keypad | Arduino |
|--------|---------|
| 1      | NC      |
| 2      | D2      |
| 3      | D3      |
| 4      | D4      |
| 5      | D5      |
| 6      | D6      |
| 7      | D7      |
| 8      | D8      |
| 9      | NC      |

Note: This pin layout means the keypad can be connected straight to the arduino via breadboard.
If you directly use wires, you need to flip one end.

## Software

If the devices are connected to the pins as described above,
the code can just be transferred to the nano and it will start working.

### Configuration

In the loop function where `ask_pin` is called,
you can change the arguments to allow an empty pin,
and to mask it using asterisks on the LCD instead of showing the digits.
By default, both options are disabled.

### Reading The Pin

The pin is sent via the USB serial connection (also the TX and RX pins).
The device will send a line `PIN xxxx` where xxxx is the pin.

### Notes on Extending the Software

When you make your own modifications, be aware of the watchdog.
You must call `ping()` at intervals less than 1 second or the device will reset.
A device reset is indicated by the backlight of the LCD briefly turning off as it reinitializes.

In general this means you should not wait for a signal from external hardware,
and if you must wait, call `ping()` inside of the wait loop.
There's no restriction as to how often and how fast you call that function.
