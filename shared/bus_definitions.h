//bus ports
#define ADDRESS_BUS PORTD
#define ADDRESS_BUS_DIRECTION TRISD

#define DATA_BUS PORTD
#define DATA_BUS_DIRECTION TRISD
// set this when using a shared address/data bus. Address bus occupies the 
// lower n bits, data bus the rest. Data bus bits are shifted down this many
// steps.
#define DATA_BUS_SHIFT 4

#define LCD_DATA_PORT PORTB
#define LCD_DATA_DIRECTION TRISB

#define LCD_CTRL_PORT PORTD
#define LCD_CTRL_DIRECTION TRISD

//bus control
#define SEL_DATA_IN 0     //enable tristate buffers and disable LED output
#define SEL_DATA_OUT 0x30 //disable tristate buffers and enable LED output
#define DATA_IN 0xFF      //set all lines to read
#define DATA_OUT 0        //set all lines to write