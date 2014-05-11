#include "keyboard.h"
#include "keyboard_private.h"

#include "../shared/key_consts.h"
#include "../shared/io.h"
#include "../command_handler.h"
#include "../shared/bus_definitions.h"

/**
 * Keyboard functions
 * ------------------
 * All keys are normally high, pulled to +5V by a 10k resistor. When a key is 
 * short circuited to ground, it is  read as a key down. When the key is 
 * returned to high, it is read as a key up.
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
 *
 * Rows are scanned, columns are read, so all columns for a row are read as
 * one byte. Columns are pulled high by a 10k resistor.
 **/

// mapping between rows/columns and key numbers
unsigned short keymap[(MATRIXROWS +1) * MATRIXCOLS];

// variables for detecting button changes.
unsigned short previousState[11];
unsigned short currentState[11];

/**
 * Initialize keyboard
 **/
void KBD_init(){
  unsigned short row;
  
  // Read initial keyboard states
  for(row = 0; row < MATRIXROWS; row++){
    IO_setAddressLineLow(row);
    delay_ms(1);
    currentState[row] = IO_readData();
    previousState[row] = currentState[row];
  }
}

void KBD_initSystemButtons(){
  currentState[SYSTEM_ROW] = 0xFF;
  previousState[SYSTEM_ROW] = 0xFF;
}

/**
 * Read all keys on the current row and raise any key pressed/released events
 **/
void KBD_read(unsigned short row){
  IO_setAddressLineLow(row);
  delay_us(200);
  KBD_triggerKeyEventsAndUpdateState(row, IO_readData());
}

/**
 * Read system specific keys (not part of the normal matrix, this may change later).
 *
 * NB: For this to work, PORTB must be pulled up near the port, and the buttons must
 * pull the port to GND.
 *
 * Button RB0 turns on/off learn mode. PS: Remember to press AND HOLD a note if
 * you want to map a note on, as a note off is sent when the key is released
 * and that will be mapped instead.
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 **/
void KBD_readSystemButtons(){
  KBD_triggerKeyEventsAndUpdateState(SYSTEM_ROW, IO_readSystemButtons());
}

void KBD_triggerKeyEventsAndUpdateState(unsigned short row, unsigned short states){
  currentState[row] = states;
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
  for( col=0; col < MATRIXCOLS; col++){
    keyPressed = states & (1<<col);

    if( keyPressed ) {
      if(row == SYSTEM_ROW){
        CMD_systemKeyEventDispatcher(col, keydirection);
      } else {
        keyCode = keymap[row*MATRIXCOLS+col]; //lookup ID of key pressed.
        CMD_keyEventDispatcher(keyCode, keydirection);
      }
    }
  }
}

/**
 * map column/row to key number. indexes are [row * MATRIXCOLS + column]
 **/
void KEYMAP_init(unsigned short keymap[]){
  unsigned short row;
  unsigned short col;
  unsigned short index;
  
  // keymap is not in use but may be implemented to disconnect key scanning
  // from key codes.
  index = 0;
  for(row = 0; row < MATRIXROWS; row++){
    for(col = 0; col < MATRIXCOLS; col++){
      keymap[index] = index;
      index++;
    }
  }
}