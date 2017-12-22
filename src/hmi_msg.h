#include <avr/pgmspace.h>
#ifndef _HMI_MSG_H_
#define _HMI_MSG_H_
#define VER_FW "Version: " FW_VERSION " built on: " __DATE__ " " __TIME__"\r\n"
#define VER_LIBC "avr-libc version: " __AVR_LIBC_VERSION_STRING__" avr-gcc version: " __VERSION__ "\r\n"

extern const char NAME[];
extern const char ENTERED[];
extern const char WRONG[];
extern const char MESSAGE_1[];
extern const char MESSAGE_2[];
extern PGM_P const list[];

#endif /* _HMI_MSG_H_ */