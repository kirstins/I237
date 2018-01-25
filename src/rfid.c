#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"
#include "../lib/helius_microrl/microrl.h"
#include "../lib/matejx_avr_lib/mfrc522.h"
#include "../lib/andy_brown_memdebug/memdebug.h"
#include "hmi_msg.h"
#include "cli_microrl.h"
#include "print_helper.h"

typedef struct card {
    char *uid;
    char *user;
    short size;
    struct card *next;
} card_t;

card_t * cardlist = NULL;

void rfid_read(const char *const *argv)
{
    (void) argv;
    Uid uid;
    uart0_puts_p(PSTR("\n\r"));

    //checking rfid cards nearby
    if (PICC_IsNewCardPresent()) {
        uart0_puts_p(PSTR("Card selected!\n\r"));
        PICC_ReadCardSerial(&uid);
        uart0_puts_p(PSTR("Card type: "));
        uart0_puts(PICC_GetTypeName(PICC_GetType(uid.sak)));
        uart0_puts_p(PSTR("\r\n"));
        uart0_puts_p(PSTR("Card UID: "));
        char hex[4];

        // Changing the bytes on the RFID card change to human-readable characters
        for (byte i = 0; i < uid.size; i++) {
            sprintf(hex, "%02X", uid.uidByte[i]);
            uart0_puts(hex);
        }

        //Output for users
        char uid_size[10];
        uart0_puts_p(PSTR(" (size " ));
        uart0_puts(itoa(uid.size, uid_size, 10));
        uart0_puts_p(PSTR(" bytes)\r\n"));
    } else {
        uart0_puts_p(PSTR("Error: could not select card.\r\n"));
    }
}

void rfid_add(const char *const *argv)
{
    uart0_puts_p(PSTR("\r\n"));
    card_t *newcard;
    newcard = malloc(sizeof(card_t));

    //Checking whether UID is no more then 10 bytes
    if ((strlen(argv[1]) > 20)) {
        uart0_puts_p(
            PSTR("UID maximum size is 10 bytes, i.e 20 hexadecimal numbers\r\n"));
        return;
    }

    // Checking whether UID already exists among list of cards
    if (cardlist != NULL) {
        newcard->next = cardlist;
        card_t *current = cardlist;

        while (current != NULL) {
            if (!strcmp(current->uid, newcard->uid)) {
                uart0_puts_p(PSTR("This card is already in the list.\r\n"));
                return;
            }

            current = current->next;
        }
    } else {
        newcard->next = NULL;
    }

    // Checking allocation of memory and adding new card to list
    newcard->uid = malloc(strlen(argv[1]) + 1);

    if (newcard->uid == NULL) {
        uart0_puts_p(PSTR("Could not allocate memory\r\n"));
        free(newcard);
        return;
    }

    strcpy(newcard->uid, argv[1]);
    newcard->size = strlen(argv[1]) / 2;
    newcard->user = malloc(strlen(argv[2]) + 1);

    if (newcard->user == NULL) {
        uart0_puts_p(PSTR("Could not allocate memory\r\n"));
        free(newcard);
        return;
    }

    strcpy(newcard->user, argv[2]);
    char uid_size[10];
    cardlist = newcard;
    uart0_puts_p(PSTR("New card added with UID: "));
    uart0_puts(newcard->uid);
    uart0_puts_p(PSTR("\r\nUID size: "));
    uart0_puts(itoa(newcard->size, uid_size, 10));
    uart0_puts_p(PSTR(" bytes\r\nUser name: "));
    uart0_puts(newcard->user);
    uart0_puts_p(PSTR("\r\n"));
}

void rfid_remove(const char *const *argv)
{
    card_t *current = cardlist;
    card_t *previous = NULL;

    if (cardlist == NULL) {
        uart0_puts("There are no cards.\r\n");
        return;
    } else {
        while (current != NULL) {
            if (!strcmp(argv[1], current->uid)) {
                uart0_puts_p(PSTR("Card with uid "));
                uart0_puts(current->uid);
                uart0_puts_p(PSTR(" is removed.\r\n"));
                free(current);
                return;
            } else {
                if (previous == NULL) {
                    cardlist = current->next;
                } else {
                    previous->next = current->next;
                }
            }

            previous = current;
            current = current->next;
        }

        uart0_puts_p(PSTR("Card not found.\n\r"));
        return;
    }
}

void rfid_print(void)
{
    uart0_puts_p(PSTR("\r\n"));

    if (cardlist == NULL) {
        uart0_puts_p(PSTR("There are no cards.\r\n"));
    } else {
        uart0_puts_p(PSTR("Cards in list:\r\n"));
        card_t *current = cardlist;
        int counter = 1;
        char counts[10];
        char uid_size[10];

        while (current != NULL) {
            itoa(counter, counts, 10);
            uart0_puts(counts);
            uart0_puts(". User: ");
            uart0_puts(current->user);
            uart0_puts(" UID: ");
            uart0_puts(current->uid);
            uart0_puts_p(PSTR(" (size of"));
            uart0_puts(itoa(current->size, uid_size, 10));
            uart0_puts_p(PSTR(" bytes)"));
            uart0_puts_p(PSTR("\r\n"));
            current = current->next;
            counter++;
        }
    }
}

const char *find(char *search_uid)
{
    card_t *current = cardlist;

    while (current != NULL) {
        if (!strcmp(current->uid, search_uid)) {
            return current->user;
        }

        current = current->next;
    }

    return NULL;
}






