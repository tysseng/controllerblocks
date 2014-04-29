#ifndef _COMMAND_HANDLER_H
#define _COMMAND_HANDLER_H
//void (*CMD_outputHandler) (char channel, char controller, char lowValue, char highValue);
void CMD_init();
void CMD_keyEventDispatcher(char key, unsigned short keydirection);
void CMD_systemKeyEventDispatcher(char col, unsigned short keydirection);
#endif