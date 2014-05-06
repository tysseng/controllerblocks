#ifndef _ANALOG_PRIVATE_H
#define _ANALOG_PRIVATE_H

#define ANALOG_TRESHOLD 3 //minimal change to read as value changed
#define ANALOG_MODE_BUTTON 0
#define ANALOG_MODE_POTENTIOMETER 1
#define ANALOG_BUTTON_TRESHOLD_ON 800

void ANALOG_triggerEvent(unsigned short row, unsigned short col, unsigned int value);
void ANALOG_generateEvent( unsigned short row, unsigned short col, unsigned int value);
void ANALOG_generateKeyEvent(unsigned short row, unsigned short col, unsigned short direction);
unsigned short ANALOG_getMode(unsigned short row, unsigned short col);
#endif