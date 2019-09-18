#include "hardware.h"


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
