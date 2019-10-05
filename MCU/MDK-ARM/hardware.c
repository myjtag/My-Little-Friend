#include "hardware.h"

#define Data_Latch		0x0001
#define Global_latch	0x0002

extern uint8_t URATRX[50];
extern uint8_t RXDcnt;
DS1307 Now;
extern I2C_HandleTypeDef hi2c1;
uint16_t RGB_DATA[16]={0};
uint8_t SegmentPatern[16]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};
uint8_t segmentBuffer[4]={0,1,2,3};
uint8_t segCnt=0;

const uint16_t GaussinPatern[128] = {0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,3,4,5,7,9,12,15,20,25,32,41,52,66,82,102,126,155,189,229,276,330,393,465,546,637,740,853,978,1114,1261,1418,1586,1762,1946,2136,2329,2525,2720,2912,3098,3276,3442,3595,3731,3849,3945,4020,4070,4095,4095,4070,4020,3945,3849,3731,3595,3442,3276,3098,2912,2720,2525,2329,2136,1946,1762,1586,1418,1261,1114,978,853,740,637,546,465,393,330,276,229,189,155,126,102,82,66,52,41,32,25,20,15,12,9,7,5,4,3,2,1,1,1,1,0,0,0,0,0,0,0,0,0,0};
	
//With this fucntion we can read the keypad
int getKeypad(void){
	int key = NoKey;
	
	//we have key press of the OK key
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12) == 0){
		HAL_Delay(10);
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12) == 0)
			key = Key_OK;
		else
			key = NoKey;
	}
	
	return key;
}

//It would write a single 16bit data into the RGB controller cheap
void STP1612PW05_SPI(uint16_t data,uint16_t LEMASK){
 	uint16_t bit;
	for(bit = 0x8000;bit; bit >>=1){
		if(bit & data)
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
		
		if(bit == LEMASK)
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);
		
		//CLK = 1
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
		//Dleay
		//for(int i=0;i<100;i++);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
	}
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
}

//It would write all the data to the all the channels
void STP1612PW05_WriteAll(void)
{
	uint16_t bitmask;
	
	for( int i=0;i<16;i++){
		if(i<15)
			bitmask = Data_Latch;
		else
			bitmask = Global_latch;
		
		STP1612PW05_SPI(RGB_DATA[15-i],bitmask);
	} 
}


void RGBtoSegment(uint8_t data){
	//Dot
	if(data & 128)
		RGB_DATA[3] = 0xFFFF;
	else
		RGB_DATA[3] = 0x0;

	//G
	if(data & 64)
		RGB_DATA[0] = 0xFFFF;
	else
		RGB_DATA[0] = 0x0;

	//F
	if(data & 32)
		RGB_DATA[5] = 0xFFFF;
	else
		RGB_DATA[5] = 0x0;
	//E
	if(data & 16)
		RGB_DATA[6] = 0xFFFF;
	else
		RGB_DATA[6] = 0x0;	
	//D
	if(data & 8)
		RGB_DATA[4] = 0xFFFF;
	else
		RGB_DATA[4] = 0x0;	
	//C
	if(data & 4)
		RGB_DATA[1] = 0xFFFF;
	else
		RGB_DATA[1] = 0x0;	
	//B
	if(data & 2)
		RGB_DATA[2] = 0xFFFF;
	else
		RGB_DATA[2] = 0x0;	
	//A
	if(data & 1)
		RGB_DATA[7] = 0xFFFF;
	else
		RGB_DATA[7] = 0x0;	
	
	STP1612PW05_WriteAll();
}
//It would update all the 7 segments
void updateSegment(void){
	
	//Turn off all common anodes
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);//C0
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);//C1
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);//C2
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);//C3
	
	//Update the desigred segment
	switch (segCnt)
	{
		case 0:
			RGBtoSegment(SegmentPatern[segmentBuffer[0]]);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
		break;
		
		case 1:
			RGBtoSegment(SegmentPatern[segmentBuffer[1]]);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
		break;
		
		case 2:
			RGBtoSegment(SegmentPatern[segmentBuffer[2]]);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
		break;	
		
		case 3:
			RGBtoSegment(SegmentPatern[segmentBuffer[3]]);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);
		break;	
	}
	
	segCnt++;
	if(segCnt>3)
		segCnt = 0;
}

void SetTime(uint8_t second,uint8_t minute,uint8_t hour,uint8_t day,uint8_t date,uint8_t month,uint8_t year){
	
	//Firt convert from binary to BCD fomrat since DS1307 would accept data in BCD
	second = ((second/10)<<4) + second %10;
	minute = ((minute/10)<<4) + minute %10;
	hour = ((hour/10)<<4) + hour %10;
	day = ((day/10)<<4) + day %10;
	date = ((date/10)<<4) + date %10;
	month = ((month/10)<<4) + month %10;
	year = ((year/10)<<4) + year %10;
	
	uint8_t  txBuf[8] ={0,second,minute,hour,day,date,month,year};
	 
	HAL_I2C_Master_Transmit(&hi2c1,DS1307ADD,txBuf,8,100);
}

void GetTime(void){
	uint8_t txBuf[1]={0};
	uint8_t rxBuf[7];
	
	//first set the RAM addres to point to 0
	HAL_I2C_Master_Transmit(&hi2c1,DS1307ADD,txBuf,1,100);
	
	//Now read all the clock and clander data
	HAL_I2C_Master_Receive(&hi2c1,DS1307ADD,rxBuf,7,100);
	//now we have all the data, we should change them back from BCD to Binary
	Now.second = ((rxBuf[0]>>4)*10) + rxBuf[0]%16;
	Now.minute = ((rxBuf[1]>>4)*10) + rxBuf[1]%16;
	Now.hour = ((rxBuf[2]>>4)*10) + rxBuf[2]%16;
	Now.day = ((rxBuf[3]>>4)*10) + rxBuf[3]%16;
	Now.date = ((rxBuf[4]>>4)*10) + rxBuf[4]%16;
	Now.month = ((rxBuf[5]>>4)*10) + rxBuf[5]%16;
	Now.year = ((rxBuf[6]>>4)*10) + rxBuf[6]%16;

};

//it would clear all the data in UART recive buffer
void clearRXD(void){

	uint8_t i=0;
	for(i=0;i<50;i++)
		URATRX[i] = 0;
	RXDcnt= 0;
}

