#include "main.h"

#define Key_OK			0
#define Key_UP			1
#define Key_Down		2
#define Key_LEFT		3
#define Key_Right		4
#define NoKey				5


//With this fucntion we can read the keypad
int getKeypad(void);

//It would write a single 16bit data into the RGB controller cheap
void STP1612PW05_SPI(uint16_t data,uint16_t LEMASK);
//It would write all the data to the all the channels
void STP1612PW05_WriteAll(void);
