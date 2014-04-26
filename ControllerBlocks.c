#include "keyboard/keyboard.h"
#include "midi/midi.h"
#include "command_handler.h"
#include "shared/bus_definitions.h"
#include "shared/io.h"
#include "shared/global.h"
#include "shared/definitions.h"

void interrupt() {

  char midiData;

/*  if(PIR1.TMR1IF){

    // TIMER 1 - internal tempo clock
    // TODO: To get a correct clock, we need to substract the time used
    // for the if clause and the setting of the interrupt.

    // 20 instruction cycles should be extracted from the pulse timer

    TMR1H = pulseTimerStartH;
    TMR1L = pulseTimerStartL;

    PIE1.TMR1IE = 1;
    PIR1.TMR1IF = 0;

    MCU_IRQ_playNotesAndStepNext(1);

  } else if(INTCON.T0IF){
    // TIMER 0 - tap tempo timer
    // if an interrupt has been triggered, it means that the user has stopped
    // tapping the tempo or is tapping too slow. Stop timer and indicate that
    // there are not enough correct samples to set a new bpm.

    switch(MCU_timer0Source){
    case TIMER0_SOURCE_TAP_TEMPO:
      G_SEQ_enoughTaps = 0;
      currentTap = -1;
      T0CON = 0x06;
      INTCON = 0xE0; // Set GIE, PEIE, T0IE, clear T0IF
      break;
    case TIMER0_SOURCE_KEY_REPEAT:
      G_CMD_repeatLastKey = 1;
      MCU_restartKeyRepeatTimer();
      break;
    case TIMER0_SOURCE_DIAL_SPEED:
      break;
    }
  } else*/ 
  
  if (PIR1.RCIF) {
    // MIDI input

    midiData = RCREG;
    if( midiData & MIDI_MASK_STATUS_BYTE ) {
      MIDI_IRQ_handleMidiStatusByte(midiData);
    } else {
      MIDI_IRQ_handleMidiDataByte(midiData);
    }

    //TODO: DO WE HAVE TO RESET INTERRUPT HERE?
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
