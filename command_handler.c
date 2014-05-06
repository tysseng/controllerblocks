#include "command_handler.h"
#include "command_handler_private.h"
#include "midi/midi.h"
#include "shared/key_consts.h"
#include "shared/bus_definitions.h"
#include "shared/global.h"
#include "keyboard.h"

unsigned short midiChannelMap[MATRIXROWS * MATRIXCOLS];
unsigned short midiStatusMap[MATRIXROWS * MATRIXCOLS];
unsigned short midiControllerMap[MATRIXROWS * MATRIXCOLS];

void CMD_init(){
  // Init command handler
  unsigned short i;
  for(i=0; i< MATRIXROWS * MATRIXCOLS; i++){
    midiChannelMap[i] = 0;
    midiStatusMap[i] = 0x90; //note on
    midiControllerMap[i] = 60 + i;
  }
}

/**
 * value should be between 0 and 127
 */
void CMD_generalEventDispatcher(char key, unsigned short value){

  MIDI_sendMidiMessage(
    midiChannelMap[key],
    midiStatusMap[key],
    midiControllerMap[key],
    value
  );
  
  delay_ms(10);
  MIDI_sendMidiMessage(
    midiChannelMap[key],
    midiStatusMap[key],
    midiControllerMap[key],
    0
  );
  
}

void CMD_keyEventDispatcher(char key, unsigned short keydirection){

  // Todo: reintroduce key repeat if necessary
  // Todo: reintroduce shift if necessary
  if(keydirection == KEYDIR_DOWN){
    CMD_dispatchKeyDown(key);
  } else {
    CMD_dispatchKeyUp(key);
  }
}

void CMD_dispatchKeyDown(char key){
  if(GLOBAL_mode == MODE_LEARN){
    if(MIDI_messageReady){
      midiChannelMap[key] = MIDI_lastChannel;
      midiStatusMap[key] = MIDI_lastStatus;
      midiControllerMap[key] = MIDI_lastData1;
      MIDI_messageReady = 0;

      SYSTEM_LED_LEARN = 0;
      delay_ms(500);
      SYSTEM_LED_LEARN = 1;
    }
  } else if(GLOBAL_mode == MODE_PLAY){
    MIDI_sendMidiMessage(
      midiChannelMap[key],
      midiStatusMap[key],
      midiControllerMap[key],
      64
    );
  }
}

void CMD_dispatchKeyUp(char key){
  if(GLOBAL_mode == MODE_LEARN){
    //insert learning code here if we decide to support different up/down statuses.
  } else if(GLOBAL_mode == MODE_PLAY){
    MIDI_sendMidiMessage(
      midiChannelMap[key],
      midiStatusMap[key],
      midiControllerMap[key],
      0
    );
  }
}

void CMD_systemKeyEventDispatcher(char key, unsigned short keydirection){
  switch(key){
    case 0: // mode toggle
      if(keydirection == KEYDIR_DOWN){
        if(GLOBAL_mode == MODE_PLAY){
          GLOBAL_mode = MODE_LEARN;
          SYSTEM_LED_LEARN = 1;
        } else {
          GLOBAL_mode = MODE_PLAY;
          SYSTEM_LED_LEARN = 0;
        }
      }
      break;
  }
}