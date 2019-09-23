#include "hardware.h"

#define Data_Latch		0x0001
#define Global_latch	0x0002

uint16_t RGB_DATA[16]={0};

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
