#include <avr/pgmspace.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "hmi_msg.h"
#include "print_helper.h"
#include "../lib/hd44780_111/hd44780.h"


#define BLINK_DELAY_MS 100

static inline void init_leds(void)
{
    /* Set port B pin 7 for output for Arduino Mega yellow LED */
    DDRB |= _BV(DDB7);
    /* Set port A pin 0 for output for Arduino Mega red LED */
    DDRA |= _BV(DDA0);
    /* Set port A pin 2 for output for Arduino Mega green LED */
    DDRA |= _BV(DDA2);
    /* Set port A pin 4 for output for Arduino Mega blue LED */
    DDRA |= _BV(DDA4);
    /* Turn off yellow LED */
    PORTB &= ~_BV(PORTB7);
}

/* Init console in UART0 and print user code info */
static inline void init_con(void)
{
    simple_uart0_init();
    stdin = stdout = &simple_uart0_io;
    fprintf_P(stdout, name);
    fprintf_P(stdout, progVer, FW_VERSION, __DATE__, __TIME__);
    fprintf_P(stdout, libVer, __AVR_LIBC_VERSION_STRING__, __VERSION__);
}

static inline void lcd_display_name(void)
{
    /*Init lcd and display name*/
    lcd_init();
    lcd_clrscr();
    lcd_puts_P(name);
}

static inline void blink_leds(void)
{
    /* Turn red led on and then off*/
    PORTA |= _BV(PORTA0);
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(PORTA0);
    _delay_ms(BLINK_DELAY_MS);
    /* Turn green led on and then off*/
    PORTA |= _BV(PORTA2);
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(PORTA2);
    _delay_ms(BLINK_DELAY_MS);
    /* Turn blue led on on and then off*/
    PORTA |= _BV(PORTA4);
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(PORTA4);
    _delay_ms(BLINK_DELAY_MS);
}

void main (void)
{
    init_con();
    init_leds();
    lcd_display_name();
    /*Prtint ASCII table */
    print_ascii_tbl (stdout);
    unsigned char cArray[128] = {0};

    for (unsigned char i = 0; i < sizeof(cArray); i++) {
        cArray[i] = i;
    }

    print_for_human(stdout, cArray, sizeof(cArray));

    while (1) {
        int input = 0;
        fprintf_P(stdout, enter);
        fscanf(stdin, "%i", &input);

        /*Print number*/

        if (input >= 0 && input <= 9) {
            fprintf_P(stdout, entered);
            fprintf_P(stdout, (PGM_P)pgm_read_word(&(list[input])));
        } else {
            fprintf_P(stdout, wrong);
        }

        blink_leds();
        /* Test assert - REMOVE IN FUTURE LABS */
        /*
         * Increase memory allocated for array by 100 chars
         * until we have eaten it all and print space between stack and heap.
         * That is how assert works in run-time.
         */
        /* End test assert */
    }
}