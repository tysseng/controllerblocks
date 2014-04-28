#ifndef _MIDI_H
#define _MIDI_H

// Positions in settings array
#define MIDI_SETTING_LISTEN_ALL_CHANNELS 0
#define MIDI_SETTING_CHANNEL 1

// Masks to mask out various midi types.
#define MIDI_MASK_STATUS_BYTE 0x80
#define MIDI_MASK_CHANNEL 0x0F // removes status bits and leaves channel bits
#define MIDI_MASK_STATUS 0xF0  // removes channel bits and leaves status bits

#define MIDI_STATUS_NOTE_OFF 0x80
#define MIDI_STATUS_NOTE_ON 0x90
#define MIDI_STATUS_SYSEX_START 0xF0
#define MIDI_STATUS_SONG_POSITION 0xF2
#define MIDI_STATUS_SONG_SELECT 0xF3
#define MIDI_STATUS_SYSEX_END 0xF7

// Playback status
#define MIDI_STATUS_MIDI_CLOCK 0xF8
#define MIDI_STATUS_MIDI_START 0xFA
#define MIDI_STATUS_MIDI_CONTINUE 0xFB
#define MIDI_STATUS_MIDI_STOP 0xFC

#define MIDI_ROUTING_IGNORE 0
#define MIDI_ROUTING_HANDLE 1

extern int MIDI_byteCounter;
extern char MIDI_lastStatus;
extern char MIDI_sysexBuffer[256];

void MIDI_init();

void MIDI_handleMidiByte(char midiByte);
void MIDI_handleSysexByte(char midiByte);
void MIDI_handleMidiMessage();

void MIDI_sendMidiMessage( unsigned short channel, unsigned short status, unsigned short controller, unsigned short value);
void MIDI_sendNoteOn( unsigned short channel, unsigned short note, unsigned short velocity );
void MIDI_sendNoteOff( unsigned short channel, unsigned short note, unsigned short velocity );
void MIDI_sendSysexStart();
void MIDI_sendSysexEnd();

void MIDI_outputHandler(char channel, char controller, char lowValue, char highValue);
#endif