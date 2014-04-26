#include "midi.h"
#include "../shared/global.h"
//#include "../shared/asm_usart.h"

int MIDI_byteCounter;
char MIDI_lastStatus;
char MIDI_sysexBuffer[256];


/**
 * Init usart and midi settings
 **/
void MIDI_init(){
  UART1_Init(31250);
  G_MIDI_isSysexAddressCorrect = 0;
  G_MIDI_shouldFlushSysexBuffer = 0;
}

/**
 * Handle a midi status byte received in the interrupt routine
 **/
void MIDI_IRQ_handleMidiStatusByte(char midiStatus){
  MIDI_lastStatus = midiStatus;
  MIDI_byteCounter = 0;

  switch(midiStatus){
  //Add cases as needed.
  case MIDI_STATUS_MIDI_CLOCK:
    //Do stuff here
    break;
  }
}

/**
 * Handle a midi data byte received in interrupt routine
 **/
void MIDI_IRQ_handleMidiDataByte(char midiData){
  MIDI_byteCounter++;

  if(MIDI_lastStatus == MIDI_STATUS_SYSEX_START){
    MIDI_IRQ_handleSysexData(midiData);
  }
}

/**
 * data import from PC using sysex. NB: NOT IMPLEMENTED
 **/
void MIDI_IRQ_handleSysexData(char sysexData){
  switch(MIDI_byteCounter){
  case 1:
    G_MIDI_shouldFlushSysexBuffer = 0;
    if(sysexData == 0){
      G_MIDI_isSysexAddressCorrect = 1;
    } else {
      G_MIDI_isSysexAddressCorrect = 0;
    }
    break;
  case 2:
    if(sysexData != G_MIDI_sysexIdByte1){
      G_MIDI_isSysexAddressCorrect = 0;
    }
    break;
  case 3:
    if(sysexData != G_MIDI_sysexIdByte2){
      G_MIDI_isSysexAddressCorrect = 0;
    }
    if(G_MIDI_isSysexAddressCorrect){
      //EEX_importMemoryFromMidiStart();
    }
    break;
  default:
    MIDI_sysexBuffer[MIDI_byteCounter-4] = sysexData;
    if(MIDI_byteCounter - 4 == 255){
      G_MIDI_shouldFlushSysexBuffer = 1;
      MIDI_byteCounter = 3;
    }
 }
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