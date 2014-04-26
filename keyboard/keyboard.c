#include "keyboard.h"
#include "keyboard_private.h"

#include "../shared/key_consts.h"
#include "../shared/io.h"
#include "../command_handler.h"
#include "../shared/bus_definitions.h"

/**
 * Keyboard functions
 * ------------------
 * All keys are normally high. When a key is short circuited to ground, it is
 * read as a key down. When the key is returned to high, it is read as a key
 * up.
 *
 * Here's how the keyboard scanning algorithm expects the grid to be connected:
 *
 *                COLUMN 1               COLUMN 2
 *                   |                     |
 *            _______| connect      _______| connect
 *           |       |             |       |
 *   SWITCH /        |     SWITCH /        |
 *          _|_      |            _|_      |
 *   DIODE  \ /      |     DIODE  \ /      |
 *          ---      |            ---      |
 *           |       |             |       |
 * ROW 1 ____|_______|_____________|_______|________________
 *        connect    | cross    connect    | cross
 *                   |                     |
 *                   |                     |
 *            _______| connect      _______| connect
 *           |       |             |       |
 *   SWITCH /        |     SWITCH /        |
 *          _|_      |            _|_      |
 *   DIODE  \ /      |     DIODE  \ /      |
 *          ---      |            ---      |
 *           |       |             |       |
 * ROW 2 ____|_______|_____________|_______|________________
 *        connect    | cross    connect    | cross
 *                   |                     |
 *                   |                     |
 *
 **/

// mapping between rows/columns and key numbers
unsigned short keymap[ROWCOUNT * COLCOUNT];

// variables for detecting button changes.
unsigned short previousState[11];
unsigned short currentState[11];

/**
 * Initialize keyboard
 **/
void KBD_init(){
  unsigned short row;
  
  // Read initial keyboard states
  for(row = 0; row < ROWCOUNT; row++){
    IO_setAddressLineLow(row);
    delay_ms(1);
    currentState[row] = IO_readData();
    previousState[row] = currentState[row];
  }

  KEYMAP_init(keymap);
}

/**
 * Read all keys on the current row and raise any key pressed/released events
 **/
void KBD_read(unsigned short row){

  //read the keyboard
  IO_setAddressLineLow(row);
  delay_us(200);
  currentState[row] = IO_readData();

  //check if a button is pressed or released.
  if( currentState[row] != previousState[row]) {
    KBD_generateEvents( row, ~currentState[row] & previousState[row], KEYDIR_DOWN ); //press
    KBD_generateEvents( row, currentState[row] & ~previousState[row], KEYDIR_UP ); //release
  }
  previousState[row] = currentState[row];
}

/**
 * Handle pressed keys. Checks each bit of a column and triggers an up or down event if the
 * bit is set.
 **/
void KBD_generateEvents( unsigned short row, unsigned short states, unsigned short keydirection ) {

  unsigned short col;
  unsigned short keyPressed;
  unsigned short keyCode;

  //loop through all column bits.
  for( col=0; col < COLCOUNT; col++){
    keyPressed = states & (1<<col);

    if( keyPressed ) {
      keyCode = keymap[row*COLCOUNT+col]; //lookup ID of key pressed.
      CMD_keyEventDispatcher(keyCode, keydirection);
    }
  }
}

/**
 * map column/row to key number. indexes are [row * COLCOUNT + column]
 **/
void KEYMAP_init(unsigned short keymap[]){
  unsigned short row;
  unsigned short col;
  unsigned short index;
  
  // keymap is not in use but may be implemented to disconnect key scanning
  // from key codes.
  index = 0;
  for(row = 0; row < ROWCOUNT; row++){
    for(col = 0; col < COLCOUNT; col++){
      keymap[index] = index;
      index++;
    }
  }
}