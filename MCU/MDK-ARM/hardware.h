#include "main.h"

#define Key_OK			0
#define Key_UP			1
#define Key_Down		2
#define Key_LEFT		3
#define Key_Right		4
#define NoKey				5


#define DS1307ADD			208
typedef struct {
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
}DS1307;

//With this fucntion we can read the keypad
int getKeypad(void);

//It would write a single 16bit data into the RGB controller cheap
void STP1612PW05_SPI(uint16_t data,uint16_t LEMASK);
//It would write all the data to the all the channels
void STP1612PW05_WriteAll(void);


//7 segment functions
//It will map RGB data to 7 segment
void RGBtoSegment(uint8_t data);
//It would update all the 7 segments
void updateSegment(void);

//RTC functions
void SetTime(uint8_t second,uint8_t minute,uint8_t hour,uint8_t day,uint8_t date,uint8_t month,uint8_t year);
void GetTime(void);

//Uart functions
//it would clear all the data in UART recive buffer
void clearRXD(void);

