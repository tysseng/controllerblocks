#ifndef _IO_H
#define _IO_H
void IO_init();
void IO_setAddressLineLow(unsigned short line);
void IO_setAddressLineHigh(unsigned short line);
unsigned short IO_readData();
unsigned int IO_readAnalogData(unsigned short col);
unsigned short IO_readSystemButtons();
#endif