#ifndef _GLOBAL_H
#define _GLOBAL_H

#define G_MIDI_isSysexAddressCorrect GLOBAL_statusBits1.F0
#define G_MIDI_shouldFlushSysexBuffer GLOBAL_statusBits1.F1

extern char GLOBAL_statusBits1;
extern char G_MIDI_sysexIdByte1;
extern char G_MIDI_sysexIdByte2;
#endif
