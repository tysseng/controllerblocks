#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#define MATRIXROWS 4 //how many rows are used for keys/leds.
#define MATRIXCOLS 4 //how many cols are used for keys/leds.
#define SYSTEM_ROW 4 //must be same as rowcount, leaves room for 8 system buttons in addition to the grid.

extern unsigned short keymap[(MATRIXROWS +1) * MATRIXCOLS];

void KBD_init();
void KBD_read(unsigned short row);
void KBD_readSystemButtons();
#endif