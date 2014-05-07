#include "analog.h"
#include "analog_private.h"
#include "../shared/io.h"
#include "../shared/key_consts.h"
#include "../shared/bus_definitions.h"
#include "../keyboard/keyboard.h"
#include "../command_handler.h"

unsigned int currentAnalogValues[MATRIXROWS][MATRIXCOLS];

void ANALOG_init(){
  unsigned short row;
  unsigned short col;
  
  ADCON0 = 1; // turn on A/D converter
  ADCON1 = 0x00; //turn on analogue input (for P18F458)

  TRISA = DATA_IN;
  
  //read initial state for all analog inputs.
  for(row = 0; row < MATRIXROWS; row++){
    IO_setAddressLineHigh(row);
    for(col = 0; col < MATRIXCOLS; col++){
      currentAnalogValues[row][col] = IO_readAnalogData(col);
    }
  }
}

/**
 * Read all inputs on the current row and raise any value changed events
 **/
void ANALOG_read(unsigned short row){

  unsigned short col;
  unsigned int currentValue;
  unsigned int newValue;
  unsigned int diff;
  
  // read all columns for this row
  IO_setAddressLineHigh(row);
  delay_us(200);

  for(col = 0; col < MATRIXCOLS; col++){
    newValue = IO_readAnalogData(col);

    currentValue = currentAnalogValues[row][col];
    if(newValue > currentValue){
      diff = newValue - currentValue;
    } else {
      diff = currentValue - newValue;
    }

    // only treat value if it has changed enough from the previously sampled
    // value, to prevent oscillation.
    if(diff > ANALOG_TRESHOLD){
      currentAnalogValues[row][col] = newValue;
      ANALOG_triggerEvent(row, col, newValue);
    } else if( currentValue != newValue && newValue < 2){ //special case for 0, necessary because of treshold or we may never reach 0.
      currentAnalogValues[row][col] = 0;
      ANALOG_triggerEvent(row, col, newValue);
    } //TODO: Special case for max value.
  }
  delay_ms(1);
}

void ANALOG_triggerEvent(unsigned short row, unsigned short col, unsigned int value){
  unsigned short analogMode = ANALOG_getMode(row, col);
  ANALOG_generateEvent(row, col, value);

  /*
  switch(analogMode){
    case ANALOG_MODE_BUTTON:
      if(value > ANALOG_BUTTON_TRESHOLD_ON){
        ANALOG_generateKeyEvent(row, col, KEYDIR_DOWN);
      } else {
        ANALOG_generateKeyEvent(row, col, KEYDIR_UP);
      }
      break;
    case ANALOG_MODE_POTENTIOMETER:
      break;
  } */
}

unsigned short ANALOG_getMode(unsigned short row, unsigned short col){
  return ANALOG_MODE_BUTTON;
}

//TODO Remove?
void ANALOG_generateKeyEvent( unsigned short row, unsigned short col, unsigned short keydirection) {

  unsigned short keyCode;
  keyCode = keymap[row*MATRIXCOLS+col]; //lookup ID of key pressed.
  CMD_keyEventDispatcher(keyCode, keydirection);
}

void ANALOG_generateEvent( unsigned short row, unsigned short col, unsigned int value) {
  unsigned short keyCode;

  keyCode = keymap[row*MATRIXCOLS+col]; //lookup ID of key pressed.
  CMD_generalEventDispatcher(keyCode, value >> 3); //NB: This only works as long as the value is 10 bit!
}