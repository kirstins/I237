#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "hmi_msg.h"
#include "print_helper.h"
#include "cli_microrl.h"
#include "rfid.h"
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"
#include "../lib/helius_microrl/microrl.h"
#include "../lib/matejx_avr_lib/mfrc522.h"

#define UART_BAUD           9600
#define UART_STATUS_MASK    0x00FF
#define BLINK_DELAY_MS  1000
#define LED_GREEN       PORTA2 // Arduino Mega digital pin 24
#define LED_RED         PORTA0 // Arduino Mega digital pin 22


microrl_t rl;
microrl_t *prl = &rl;

time_t last_time;
time_t changed_time = -2;
bool default_state = false;
char last_uid[20];
char uid_string[20];


static inline void init_leds(void)
{
    /* Set pin 2 of PORTA (ARDUINO mega pin 24) and pin 0 of PORTA (ARDUINO mega pin 22)
     for output for Arduino Mega green and red LED respectively*/
    DDRA |= _BV(LED_GREEN);
    DDRA |= _BV(LED_RED);
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

static inline void init_rfid_reader(void)
{
    MFRC522_init();
    PCD_Init();
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


//Three different door states
void update_state(int door_state, char *user)
{
    switch (door_state) {
    case 0:
        lcd_clrscr();
        lcd_puts(NAME);
        lcd_goto(0x40);
        lcd_puts("Closed");
        PORTA &= ~_BV(LED_RED);
        default_state = true;
        break;

    case 1:
        lcd_clrscr();
        lcd_puts(NAME);
        lcd_goto(0x40);
        lcd_puts("Opened: ");
        lcd_puts(user);
        PORTA &= ~_BV(LED_RED);
        default_state = false;
        break;

    default:
        lcd_clrscr();
        lcd_puts(NAME);
        lcd_goto(0x40);
        lcd_puts("Denied");
        PORTA &= ~_BV(LED_RED);
        default_state = false;
        break;
    }
}

//
void handle_rfid()
{
    Uid uid;
    Uid *uid_ptr = &uid;

    if (PICC_IsNewCardPresent()) {
        strcpy(uid_string, "");
        PICC_ReadCardSerial(uid_ptr);

        for (byte i = 0; i < uid.size; i++) {
            char uid_s[20];
            itoa(uid.uidByte[i], uid_s, 10);
            strcat(uid_string, uid_s);
        }

        //setting the timestamp that the card has seen to present time
        last_time = time(NULL);
    }

    // if more than 1 second has past, variable last_uid and uid_string will be cleared
    if (time(NULL) - last_time > 1) {
        strcpy(last_uid, "");
        strcpy(uid_string, "");
    }

    //if variables last_uid and uid_string are not null
    if (strcmp(last_uid, uid_string) != 0) {
        char *user = find(uid_string);  //find the user whom the UID belongs

        if (user == NULL) {          //if there are no such user, deny access
            update_state(2, NULL);
        } else {                     //otherwise open the door
            update_state (1, user);
        }

        strcpy(last_uid, uid_string);
    }

    //if at least 2 seconds have gone forward, check the default state of the door
    if (time(NULL) - changed_time >= 2) {
        if (!default_state) {
            update_state(0, NULL); //if the door is in default state, close it
        }
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
    MFRC522_init();
    PCD_Init();
    sei();

    while (1) {
        heartbeat();
        microrl_insert_char(prl, (uart0_getc() & UART_STATUS_MASK));
        handle_rfid();
    }
}

ISR(TIMER1_COMPA_vect)
{
    system_tick();
}
