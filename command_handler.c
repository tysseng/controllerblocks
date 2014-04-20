#include "command_handler.h"
#include "shared/key_consts.h"
#include "shared/bus_definitions.h"

void CMD_init(){
  // Init command handler
}

void CMD_dispatchKeyDown(char key){
  LCD_DATA_PORT = key;
  // Do something with that key
}

void CMD_dispatchKeyUp(char key){
//  LCD_DATA_PORT = 0;
  // Do something with that key
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