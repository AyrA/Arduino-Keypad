/*
  Handles the 3x4 matrix keypad.
  The 9 pins at the bottom are as follows from left to right:
  1: Not connected
  2: Column 2
  3: Row    1
  4: Column 1
  5: Row    4
  6: Column 3
  7: Row    3
  8: Row    2
  9: Not connected

  Note: The default configuration is made for a keypad that is directly plugged into the arduino.
  If yours is not (for example it's connected via cable or breadboard) you need to remap the pins in kp_rows and kp_cols,
  or you keep the values and twist the cable 180°
  The pins in use by default are D2 to D8
*/

//Character used when no key is pressed
#define KP_NOPRESS 'X'
//Characrer used when multiple keys are pressed
#define KP_MULTIPRESS 'M'

//Pins that connect to the 4 rows
const int kp_rows[] = {3, 8, 7, 5};
//Pins that connect to the 3 columns
const int kp_cols[] = {4, 2, 6};
//Serial charset in the order the columns are probed
const char kp_charset[] = "#9630852*741";

//Reads the keypad and returns the pressed character
char keypad_read() {
  unsigned int ret = 0;
  for (int i = 0; i < 3; i++) {
    //Pins are set to "INPUT_PULLUP", meaning a 1 is the default state.
    //We set a column low and then check if any of the rows also goes low.
    //If a row goes low, the key that intersects the given column and row is held down.
    //We do this for all 3 columns and then check all 4 rows, giving us 12 states.
    //We store them as bits in a single integer
    digitalWrite(kp_cols[i], LOW);
    for (int j = 0; j < 4; j++) {
      ret <<= 1;
      ret |= digitalRead(kp_rows[j]) ? 0 : 1;
    }
    digitalWrite(kp_cols[i], HIGH);
  }
  int kpcheck = 0;
  //"ret" being non-zero means at least one key is pressed
  while (ret) {
    //Check if the lowest bit is set
    if (ret & 1) {
      //Ensure that only the lowest bit is set. If it's not, multiple keys are pressed
      if (ret != 1) {
        //Multiple simultaneous presses are rejected because some keys disconnect other keys,
        //meaning we cannot reliably detect a multi-press
        return KP_MULTIPRESS;
      }
      //Return the appropriate character from the character table
      return kp_charset[kpcheck];
    }
    //Continue to check the next bit
    ret >>= 1;
    ++kpcheck;
  }
  //No key is being pressed
  return KP_NOPRESS;
}

//Initializes the keypad pins
void keypad_init() {
  for (int i = 0; i < 4; i++) {
    pinMode(kp_rows[i], INPUT_PULLUP);
  }
  for (int i = 0; i < 3; i++) {
    pinMode(kp_cols[i], OUTPUT);
    digitalWrite(kp_cols[i], HIGH);
  }
}

//Checks if the keypad has a valid input (exactly one key held down)
bool keypad_has_valid_input() {
  char c = keypad_read();
  return c != KP_NOPRESS && c != KP_MULTIPRESS;
}

//Checks if no key is held down
bool keypad_is_released() {
  return keypad_read() == KP_NOPRESS;
}
