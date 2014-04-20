#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#define ROWCOUNT 4 //how many rows are used for keys/leds.
#define COLCOUNT 4 //how many cols are used for keys/leds.

extern unsigned short keymap[ROWCOUNT * COLCOUNT];

void KBD_init();
void KBD_read(unsigned short row);
#endif