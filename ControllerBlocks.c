#include "keyboard/keyboard.h"
#include "command_handler.h"
#include "shared/bus_definitions.h"
#include "shared/io.h"

/**
 * Set special registers necessary for proper operation, as well as permanent
 * port directions for address lines
 **/
void MCU_init(){

  //Necessary to access individual bits on ports
  ADCON0 = 0; // turn off A/D converter
  CMCON |= 0x07; //turn off comparators

  ADCON1 = 0x07; //turn off analogue inputs (for P18F458)
  //ADCON1 = 0x0F;   //turn off analogue inputs (for P18F4620)
  //CCP1CON = 0 ;    //disable the enhanced capture/PWM module (for P18F4620)
  
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
  KBD_init();
  CMD_init();

  row = 0;
  while(1){
    KBD_read(row);
    row = (row + 1) % ROWCOUNT;
  }
}