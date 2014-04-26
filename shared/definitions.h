#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

//External storage
#define DONT_SEND_ACK 0
#define SEND_ACK 1

//Controller timers
#define TIMER0_SOURCE_TAP_TEMPO 0
#define TIMER0_SOURCE_KEY_REPEAT 1
#define TIMER0_SOURCE_DIAL_SPEED 2

// interrupt control
#define DISABLE_ALL_INTERRUPTS INTCON.GIE=0
#define ENABLE_ALL_INTERRUPTS INTCON.GIE=1

#endif