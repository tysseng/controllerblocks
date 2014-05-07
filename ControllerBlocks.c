#include "keyboard/keyboard.h"
#include "analog/analog.h"
#include "midi/midi.h"
#include "command_handler.h"
#include "shared/bus_definitions.h"
#include "shared/io.h"
#include "shared/global.h"
#include "shared/definitions.h"

/**
Instructions:

LEARN
-----
To learn midi commands, press RB0. RE0 will light up and the controller is ready
to receive midi messages. Send the desired midi message, then press the key you
want to send that message. RE0 should turn off for half a second and then back
on again. Once RE0 is on again, repeat the process for each desired key. Should 
you want to bind another controller key to the same midi message, you'll have to
send the same midi message again.

When you have finished learning all keys, press RB0 again to switch off learning 
mode.

NB: Remember that a piano key will also send a note off. The midi message bound
to the corresponding controller key is the LAST midi message received before
the key was pressed. To learn a note on, press AND HOLD the piano key while
pressing the desired controller key.
**/

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

  //enable interrupts. NB: PEIE/GIE also affects timer0/1
  PEIE_bit = 1;  //TODO: Check if necessary
  GIE_bit = 1;   //TODO: Check if necessary
  RCIE_bit = 1;  // enable USART RX interrupt

  TRISE.F1 = DATA_OUT; //debug led
  PORTE.F1 = 0;
  
  TRISC = DATA_OUT;
  PORTC = 0;
  
  SYSTEM_LED_LEARN_DIRECTION = DATA_OUT;
  SYSTEM_LED_LEARN = 0;
}

void main() {

  unsigned short row = 0;
  
  MCU_init();
  IO_init();
  MIDI_init();
  KBD_initSystemButtons();
  KEYMAP_init(keymap);
  CMD_init();
  ANALOG_init();
  
  GLOBAL_mode = MODE_PLAY;

  row = 0;
  while(1){
    if(row == 0){
      KBD_readSystemButtons();
    }
    ANALOG_read(row);
    row = (row + 1) % MATRIXROWS;
    PORTC.F2 = MIDI_messageReady;
  }
}

/***
The big todo/todecide:
- Setup
  - Via 'config' button and send midi/touch controller to pair
  - Via menu / sysex
    - Set default value
    - Set running status on/off
  - On config: Try to detect if this is a button or a potentiometer. Blink
    the button or pot led to show what was detected. Let the user change this
    with a toggle mode button, step to next type and blink that led.
- Analog input
  - debouncing - +/- 2-3 is not a change
  - unused inputs will be pulled to ground so probably no need to do anything
    to them.
  - detect if input is pot or button - when learning, measure several times,
    if little change -> button, if lots of change: pot.
- Rotary encoder input
- running status - allow, make configurable?
- sysex send, inc 14 bit controllers?
- support pitch bend? How?
- support coarse/fine (14 bit) controllers?
- LCD display
- Reimplement buttons with default 0/pushed 1

**/

/**
Ideas:
- Instead of a 4 x 4 grid where spaces may be left open, only connect
  rows vertically (+ current column) and columns horizontaly (+ current row).
  This makes it possible to use less connectors for a bigger grid. Optionally:
  Use a two row header on the sides, can be connected if one wants to connect
  both rows and cols to the first block.
- Enable mapping of key up-events to different message? What value to use?
- Four x four grid only, but with built in serial? And posibility to use a cable 
  to connect more? Can connect four matrices from one controller directly,
  then even more through serial - 
- One grid connects directly to the controller and three others through cable.
- Controller-to-board cable contains 12 wires - power, serial rows and cols.
- Or: Leave serial alone and use
- Or only four x four grid, with controller connected through serial? Makes it
  possible to make smaller blocks
- Anti-flood mode - minimum time between midi send to prevent flooding old
  synths?
- Multiplex rows and cols. Would require 1 analog input and 6 digital outputs to
  read 8 x 8 grid, but requires an additional two chips, one 1-of-8 analog mux
  and one 1-of-8 digital mux, freeing up 9 pins. OR: Use shift register instead
  of digital mux, frees up an additional 1 pin making it possible to control
  everything AND use reference voltages on port A.
**/

/**
Questions:
- How to handle running status for channel pressure and program change
**/

/**
Facts:
- time multiplexing of pots won't work if pot is > 5-10k. To circumvent this,
  we need to add an external ADC.
**/