#ifndef _HMI_MSG_H_
#define _HMI_MSG_H_
#include <avr/pgmspace.h>

const char name[] PROGMEM = "Kirstin Saluveer\n";

const char progVer[] PROGMEM = "\nVersion: %s built on: %s %s";
const char libVer[] PROGMEM ="\navr-libc version: %s avr-gcc version: %s\n";

const char enter[] PROGMEM =  "\nEnter a number >";
const char entered[] PROGMEM = "\nYou entered number: ";
const char wrong[] PROGMEM = "Enter a number between 0 and 9.\n";

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
                              nine};



#endif /* _HMI_MSG_H_ */