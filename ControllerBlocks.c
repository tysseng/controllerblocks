#include "keyboard/keyboard.h"
#include "midi/midi.h"
#include "command_handler.h"
#include "shared/bus_definitions.h"
#include "shared/io.h"
#include "shared/global.h"
#include "shared/definitions.h"

void interrupt() {
  if (PIR1.RCIF) {
    // MIDI input
    MIDI_handleMidiByte(RCREG);
  }
}





/**
 * Set special registers necessary for proper operation
 **/
void MCU_init(){

  //Necessary to access individual bits on ports
  ADCON0 = 0; // turn off A/D converter
  CMCON |= 0x07; //turn off comparators
  ADCON1 = 0x07; //turn off analogue inputs (for P18F458)

  //enable interrupts. NB: PEIE/GIE also affects timer0/1
  PEIE_bit = 1;  //TODO: Check if necessary
  GIE_bit = 1;   //TODO: Check if necessary
  RCIE_bit = 1;  // enable USART RX interrupt

  
  LCD_DATA_DIRECTION = DATA_OUT;
  
  TRISE.F0 = DATA_OUT; //debug led 1
  TRISE.F1 = DATA_OUT; //debug led 2
  PORTE.F0 = 1;
  PORTE.F1 = 1;
}

void main() {
  unsigned short row = 0;
  
  MCU_init();
  IO_init();
  MIDI_init();
  KBD_init();
  CMD_init();

//  CMD_outputHandler = MIDI_outputHandler;

  row = 0;
  while(1){
    KBD_read(row);
    row = (row + 1) % ROWCOUNT;
  }
}


/***
The big todo/todecide:
- Setup
  - Via 'config' button and send midi/touch controller to pair
  - Via menu / sysex
    - Set default value
    - Set running status on/off
- Analog input
- Rotary encoder input
- running status - allow, make configurable?
- sysex send, inc 14 bit controllers?
- support pitch bend? How?
- LCD display

**/

/**
Ideas:
- Instead of a 4 x 4 grid where spaces may be left open, only connect
  rows vertically (+ current column) and columns horizontaly (+ current row).
  This makes it possible to use less connectors for a bigger grid. Optionally:
  Use a two row header on the sides, can be connected if one wants to connect
  both rows and cols to the first block.
**/

/**
Questions:
- How to handle running status for channel pressure and program change
**/