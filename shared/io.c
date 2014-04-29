#include "bus_definitions.h"
#include "io_private.h"

void IO_init(){
  //shared data and address bus
  ADDRESS_BUS_DIRECTION.F0 = DATA_OUT;
  ADDRESS_BUS_DIRECTION.F1 = DATA_OUT;
  ADDRESS_BUS_DIRECTION.F2 = DATA_OUT;
  ADDRESS_BUS_DIRECTION.F3 = DATA_OUT;
  DATA_BUS_DIRECTION.F4 = DATA_IN;
  DATA_BUS_DIRECTION.F5 = DATA_IN;
  DATA_BUS_DIRECTION.F6 = DATA_IN;
  DATA_BUS_DIRECTION.F7 = DATA_IN;
  SYSTEM_BUTTON_DIRECTION = DATA_IN;
}

void IO_setAddressLineLow(unsigned short line){
  ADDRESS_BUS = 0x0F ^ (1 << line);
}

unsigned short IO_readData(){
  return DATA_BUS >> DATA_BUS_SHIFT;
}

unsigned short IO_readSystemButtons(){
  return SYSTEM_BUTTON_PORT & SYSTEM_BUTTON_MASK;
}