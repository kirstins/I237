#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "hmi_msg.h"
#include "cli_microrl.h"
#include "print_helper.h"
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"
#include "../lib/helius_microrl/microrl.h"

#define UART_BAUD           9600
#define UART_STATUS_MASK    0x00FF
#define BLINK_DELAY_MS  1000
#define LED_GREEN       PORTA2 // Arduino Mega digital pin 24


microrl_t rl;
microrl_t *prl = &rl;


static inline void init_leds(void)
{
    /* Set pin 2 of PORTA (ARDUINO mega pin 24) for output for Arduino Mega green LED and set it low */
    DDRA |= _BV(LED_GREEN);
    PORTA &= ~(_BV(LED_GREEN));
}


static inline void init_con_uarts(void)
{
    uart0_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    uart1_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    uart1_puts_p(PSTR("Console started\r\n"));
}


static inline void init_con_cli(void)
{
    uart0_puts_p(NAME);
    uart0_puts_p(MESSAGE_1);
    uart0_puts_p(MESSAGE_2);
    //Call init with ptr to microrl instance and print callback
    microrl_init(prl, uart0_puts);
    //Set callback for execute
    microrl_set_execute_callback(prl, cli_execute);
}


static inline void init_sys_timer(void)
{
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= _BV(WGM12); // Turn on CTC (Clear Timer on Compare)
    TCCR1B |= _BV(CS12); // fCPU/256
    OCR1A = 62549; // Note that it is actually two registers OCR5AH and OCR5AL
    TIMSK1 |= _BV(OCIE1A); // Output Compare A Match Interrupt Enable
}


static inline void heartbeat(void)
{
    static time_t prev_time;
    char ascii_buf[11] = {0x00};
    time_t now = time(NULL);

    if (prev_time != now) {
        //Print uptime to uart1
        ltoa(now, ascii_buf, 10);
        uart1_puts_p(PSTR("Uptime: "));
        uart1_puts(ascii_buf);
        uart1_puts_p(PSTR(" s.\r\n"));
        //Toggle LED
        PORTA ^= _BV(LED_GREEN);
        prev_time = now;
    }
}

void main (void)
{
    init_leds();
    init_con_uarts();
    init_sys_timer();
    lcd_init();
    lcd_puts_P(NAME);
    init_con_cli();
    sei();

    while (1) {
        heartbeat();
        microrl_insert_char(prl, (uart0_getc() & UART_STATUS_MASK));
    }
}

ISR(TIMER1_COMPA_vect)
{
    system_tick();
}
