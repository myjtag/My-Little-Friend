#include "hardware.h"

#define Data_Latch		0x0001
#define Global_latch	0x0002

uint16_t RGB_DATA[16]={0};
uint8_t SegmentPatern[16]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};
uint8_t segmentBuffer[4]={0,1,2,3};
uint8_t segCnt=0;
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
