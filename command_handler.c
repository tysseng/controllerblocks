#include "command_handler.h"
#include "midi/midi.h"
#include "shared/key_consts.h"
#include "shared/bus_definitions.h"
#include "keyboard.h"

unsigned short midiChannelMap[ROWCOUNT * COLCOUNT];
unsigned short midiStatusMap[ROWCOUNT * COLCOUNT];
unsigned short midiControllerMap[ROWCOUNT * COLCOUNT];

void CMD_init(){
  // Init command handler
  unsigned short i;
  for(i=0; i< ROWCOUNT * COLCOUNT; i++){
    midiChannelMap[i] = 0;
    midiStatusMap[i] = 0x90; //note on
    midiControllerMap[i] = 60 + i;
  }
}

void CMD_dispatchKeyDown(char key){
  LCD_DATA_PORT = key;
  MIDI_sendMidiMessage(
    midiChannelMap[key],
    midiStatusMap[key],
    midiControllerMap[key],
    64
  );
}

void CMD_dispatchKeyUp(char key){
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