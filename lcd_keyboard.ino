/*
   Code for a pin pad with LCD display.
   Entered pins are sent over the serial port.
   A watchdog resets the program should it fail.
*/
#include <LiquidCrystal_I2C.h>
#include <WatchDog.h>
#include "keypad.h"

//Maximum permitted pin length
#define MAX_PIN_LENGTH (10)
//Offset on the display where displaying of the pin begins
#define DSP_PIN_OFFSET (8)

//(I2C address, cols, rows)
//Expected I2C pins are SCL=A5, SDA=A4
LiquidCrystal_I2C lcd(0x27, 20, 4);

//Holds entered digits
static char pin[MAX_PIN_LENGTH + 1];
//Current offset of pin
static int pin_ptr = 0;

//Performs a soft reset
void cpu_reset() {
  WatchDog::stop();
  __asm__ volatile ("JMP 0");
}

//Performs a soft reset but waits for serial data to flush
void reset() {
  Serial.println("RESET");
  Serial.flush();
  Serial.end();
  cpu_reset();
}

//Resets the watchdog timer
void ping() {
  WatchDog::stop();
  WatchDog::start();
}

//Resets and clears the display
void lcd_reset() {
  lcd.noBlink();
  lcd.noCursor();
  lcd.backlight();
  lcd.clear();
}

//Clear the pin from memory
void clear_pin() {
  for (int i = 0; i <= MAX_PIN_LENGTH; i++) {
    pin[i] = 0;
  }
  pin_ptr = 0;
}

//Adds a digit to the pin if it's not full
//Returns false if the value is not a digit, or the pin is full
bool add_pin(char c) {
  if (c >= '0' && c <= '9' && pin_ptr < MAX_PIN_LENGTH) {
    pin[pin_ptr++] = c;
    return true;
  }
  return false;
}

//Asks for a pin
//mask: true=show pin as asterisks, otherwise show digits
//permitEmpty: true=accept "#" input if pin is still empty
bool ask_pin(bool mask, bool permitEmpty) {
  char current = 0;
  clear_pin();
  lcd_reset();
  lcd.setCursor(0, 1);
  lcd.print("[0-9] \x7E Enter pin");
  lcd.setCursor(0, 2);
  lcd.print("[*]   \x7E Clear");
  lcd.setCursor(0, 3);
  lcd.print("[#]   \x7E OK");
  lcd.setCursor(0, 0);
  lcd.print("PIN:  \x7E ");
  //Show blinking cursor to indicate that we're ready for user input
  lcd.cursor();
  lcd.blink();
  do {
    if (keypad_has_valid_input()) {
      current = keypad_read();
      if (current == '*') { //Clear
        if (pin_ptr > 0) {
          clear_pin();
          lcd.setCursor(DSP_PIN_OFFSET, 0);
          lcd.print("          ");
          lcd.setCursor(DSP_PIN_OFFSET, 0);
        }
      }
      else if (add_pin(current)) { //Try to add next digit
        lcd.print(mask ? '*' : pin[pin_ptr - 1]);
      }
      //Lock up until all keypad keys are relased.
      //This serves as debouncing routine as well as
      do {
        delay(50);
        ping();
      } while (!keypad_is_released());
    }
    ping();
  } while ((!permitEmpty && pin_ptr == 0) || current != '#'); //Stay in loop until "#" is pressed
  //Disable blinking cursor
  lcd.noCursor();
  lcd.noBlink();
  return pin_ptr > 0;
}

//Initialize watchdog and hardware
void setup() {
  WatchDog::init(cpu_reset, OVF_4000MS); //A second is not long enough for hardware initialization
  WatchDog::start();
  Serial.begin(9600);
  Serial.println("READY");

  lcd.init();
  lcd_reset();
  keypad_init();
  WatchDog::stop();
  WatchDog::setPeriod(OVF_1000MS);
  WatchDog::start();
}

//Infinitely ask for a pin and send it over serial
void loop() {
  if (ask_pin(false, false)) {
    Serial.print("PIN ");
    Serial.println(pin);
  }
}
