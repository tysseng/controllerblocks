#include "midi.h"
#include "../shared/global.h"
//#include "../shared/asm_usart.h"

int MIDI_byteCounter;
char MIDI_lastChannel;
char MIDI_lastStatus;
char MIDI_lastData1;
char MIDI_lastData2;
char MIDI_inSysexMode;
char MIDI_sysexForThisDevice;
char MIDI_sysexAddress[3];
char MIDI_sysexBuffer[256];
char MIDI_sysexByteCounter;
char MIDI_routing;
char MIDI_messageReady; // 1 if a complete message has been received.

char MIDI_settings[8];

/**
 * Init usart and midi settings
 **/
void MIDI_init(){
  UART1_Init(31250);
  MIDI_settings[MIDI_SETTING_LISTEN_ALL_CHANNELS] = 0;
  MIDI_settings[MIDI_SETTING_CHANNEL] = 0;
}

/**
 * Handle a single midi byte.
 **/
void MIDI_handleMidiByte(char midiByte){

  char channel;

  if(midiByte.F7 == 1){ //status byte
    channel = midiByte & MIDI_MASK_CHANNEL;
    
    // check that message is for this device
    // TODO: Move to buffer-populator if a buffer is used.
    if(!(
         MIDI_settings[MIDI_SETTING_LISTEN_ALL_CHANNELS] ||
         midiByte >= 0xF0 ||                         //statuses above 0xF0 (sysex) affects all channels.
         channel == MIDI_settings[MIDI_SETTING_CHANNEL]) // message is for the currently chosen channel
      ){
      MIDI_routing = MIDI_ROUTING_IGNORE;
      return;
    } 

    MIDI_messageReady = 0;
    MIDI_routing = MIDI_ROUTING_HANDLE;
    MIDI_lastStatus = midiByte < 0xF0 ? (midiByte & MIDI_MASK_STATUS) : midiByte; //stip address from status byte if present.
    MIDI_lastChannel = channel;
    MIDI_byteCounter = 1;

    // Handle sysex status messages.
    if(midiByte == MIDI_STATUS_SYSEX_START){
      MIDI_sysexByteCounter = 0;
      MIDI_inSysexMode = 1;
      MIDI_sysexForThisDevice = 1; //will be set to false if address check fails later.
    } else if(midiByte == MIDI_STATUS_SYSEX_END){
      // TODO: Do something with sysex bytes? Can also be done during treatSysexByte.
      MIDI_inSysexMode = 0;
    } else {
      //sysex aborted if in sysex mode
      MIDI_inSysexMode = 0;
    }
    
    //TODO: Handle other one-byte midi messages.
    
  } else { // data byte
    if(MIDI_routing != MIDI_ROUTING_HANDLE){
      return;
    }

    if(MIDI_inSysexMode){
      // check if sysex is meant for this device (Three byte address). This check will
      // run for all the three first sysex data bytes.
      if(MIDI_sysexByteCounter < 3){
        if(!MIDI_sysexAddress[MIDI_sysexByteCounter]){ //TODO: I can't remember how sysexAddress is populated.
          MIDI_sysexForThisDevice = 0;
        }
      } else {
        if(MIDI_sysexForThisDevice){
          MIDI_handleSysexByte(midiByte);
        }
      }
      MIDI_sysexByteCounter++;
    } else {
      //Store last received first-parameter. switch to 1 after second param to allow for running status
      //TODO: Add support for 1-data byte parameters ("Program change" and "Channel pressure")
      if(MIDI_byteCounter == 1){ // first parameter received
        MIDI_lastData1 = midiByte;
        MIDI_byteCounter = 2;
        //TODO: Trigger action/don't reset if status only requires one data byte
        MIDI_messageReady = 0; // reset to allow for running status.
      } else if(MIDI_byteCounter == 2){ // second parameter received
        MIDI_lastData2 = midiByte;
        MIDI_handleMidiMessage(); // a complete 3 byte message has been received, now go do something with it.
        MIDI_byteCounter = 1;
        MIDI_messageReady = 1;
      }
    }
  }
}

//TODO: Code copied from MPG-200, reimplement for ControllerBlocks
void MIDI_handleSysexByte(char midiByte){
  /*
  if(sysexDataCounter == 0){
    //first data byte, signals type of operation
    currentSysexOperation = midiByte;
    if(midiByte == SYSEX_OP_WRITE_SETTINGS_TO_EE){
      writeSettingsToEE();
    } else if(midiByte == SYSEX_OP_CLEAR_SETTINGS_FROM_EE){
      clearSettingsFromEE();
    } else if(midiByte == SYSEX_OP_CHANGE_SETTING){
      //do nothing, need more data before we can change settings.
      sysexDataCounter++;
    }
  } else if(sysexDataCounter == 1){
    if(currentSysexOperation == SYSEX_OP_CHANGE_SETTING){
      // read position in settings array to change
      currentSysexParam1 = midiByte;
    }
    sysexDataCounter++;
  } else if(sysexDataCounter == 2){
    if(currentSysexOperation == SYSEX_OP_CHANGE_SETTING){
      // change settings value at position indicated by previous sysex parameter.
      settings[currentSysexParam1] = midiByte;
    }
    sysexDataCounter++;
  }*/
}

void MIDI_handleMidiMessage(){
  //Do something, light up a led or whatnot.
}

/**
 * Send any kind of "normal" midi message, i.e. all messages except sysex
 * and pitch bend.
 * - controller is either the controller or the note to send. It is ignored for
 *   one and zero byte data messages.
 * - value is the velocity or controller value, usually the value read from the
 *   input block.
 **/
void MIDI_sendMidiMessage( unsigned short channel, unsigned short status, unsigned short controller, unsigned short value){

  //Status messages in the F-range are global (no channel)
  if(status < 0xF0){
    UART1_Write(status + channel);
  } else {
    UART1_Write(status);
  }

  //Channel pressure (0xDn) and Program change (0xCn) only use one data byte
  if(status < 0xC0){
    UART1_Write(controller);
  }

  //Status messages in the F-range have no data bytes
  if(status < 0xF0) {
    UART1_Write(value);
  }
}

/**
 * Send a single midi 'note on'
 **/
void MIDI_sendNoteOn( unsigned short channel, unsigned short note, unsigned short velocity ) {
  MIDI_sendMidiMessage( channel, 0x90, note, velocity);
}

/**
 * Send a single midi 'note off'
 **/
void MIDI_sendNoteOff( unsigned short channel, unsigned short note, unsigned short velocity ) {
  MIDI_sendMidiMessage( channel, 0x90, note, velocity);
}

void MIDI_sendSysexStart(){
/*
  ASM_USART_Write(MIDI_STATUS_SYSEX_START);
  ASM_USART_Write(0x00);
  ASM_USART_Write(G_MIDI_sysexIdByte1);
  ASM_USART_Write(G_MIDI_sysexIdByte2);
*/
}

void MIDI_sendSysexEnd(){
//  ASM_USART_Write(MIDI_STATUS_SYSEX_END);
}

/**
 * Receives output from the generic command handler and transforms it into midi
 * and transmits it.
 **/
void MIDI_outputHandler(char channel, char controller, char lowValue, char highValue){

}