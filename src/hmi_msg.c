#include "hmi_msg.h"

const char NAME[] PROGMEM = "Kirstin Saluveer\r\n";
const char ENTERED[] PROGMEM = "You entered number: ";
const char WRONG[] PROGMEM = "Enter a number between 0 and 9.\r\n";
const char MESSAGE_1[] PROGMEM =
    "Use backspace to delete and entry to confirm.\r\n";
const char MESSAGE_2[] PROGMEM =
    "Arrow key's and del do not work currently.\r\n";

const char null[] PROGMEM = "Zero";
const char one[] PROGMEM = "One";
const char two[] PROGMEM = "Two";
const char three[] PROGMEM = "Three";
const char four[] PROGMEM = "Four";
const char five[] PROGMEM = "Five";
const char six[] PROGMEM = "Six";
const char seven[] PROGMEM = "Seven";
const char eight[] PROGMEM = "Eight";
const char nine[] PROGMEM = "Nine";

PGM_P const list[] PROGMEM = {null,
                              one,
                              two,
                              three,
                              four,
                              five,
                              six,
                              seven,
                              eight,
                              nine
                             };

