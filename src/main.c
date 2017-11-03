#include <avr/io.h>
#include <util/delay.h>

#define BLINK_DELAY_MS 800

/*Function to turn the LED on and then off.
The colour of the LED depends on the particular pin that is set high on port A */
void blink (int x)
{
    PORTA |= x;
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~x;
    _delay_ms(BLINK_DELAY_MS);
}

void main (void)
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

    /*Loop that cycles through the three colours*/
    while (1) {
        //red
        blink(_BV(PORTA0));
        //green
        blink(_BV(PORTA2));
        //blue
        blink(_BV(PORTA4));
    }
}