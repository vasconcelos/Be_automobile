#include "MPC5604B_M27V.h"
#include "GPIO.h"

/*
Registre PCR contient la configuration des pins :

| 0 |    1    |      2      | 3 | 4  |  5  |   6   |   7  | 8 | 9 |      10                | 11 | 12 |   13   |14 | 15|
| / |Safe Mode|Analog Input?| / | GPIO[00] |Output?|Input?| / | / |PushPull 0/ OpendDrain 1| /  | /  |Slewrate|pullup?|

*/
int configurer_GPIO(int indice_PCR_register,int mode)
{
	switch (mode)
	{
		case BINARY_INPUT :
			//|0|1|2|3| |4|5|6|7| |8|9|10|11| |12|13|14|15|
			//|0|0|0|0| |0|0|0|1| |0|0| 0| 0| | 0| 0| 0| 0|
			//    0          1          0           0
			SIU.PCR[indice_PCR_register].R = 0x0100; 
			return 0;
		break;
		
		case ANALOG_INPUT :
			//|0|1|2|3| |4|5|6|7| |8|9|10|11| |12|13|14|15|
			//|0|0|1|0| |0|0|0|1| |0|0| 0| 0| | 0| 0| 0| 0|
			//    2          1          0           0
			SIU.PCR[indice_PCR_register].R = 0x2100;  
			return 0;
		break;
		
		case PUSH_PULL_OUTPUT :
			//|0|1|2|3| |4|5|6|7| |8|9|10|11| |12|13|14|15|
			//|0|0|0|0| |0|0|1|0| |0|0| 0| 0| | 0| 0| 0| 0|
			//    0          2          0           0
			SIU.PCR[indice_PCR_register].R = 0x0200; 
			return 0;
		break;
		
		case OPEN_DRAIN_OUTPUT :
			//|0|1|2|3| |4|5|6|7| |8|9|10|11| |12|13|14|15|
			//|0|0|0|0| |0|0|1|0| |0|0| 1| 0| | 0| 0| 0| 0|
			//    0          2          2           0 
			SIU.PCR[indice_PCR_register].R = 0x0220; 
			return 0;
		break;
		
		default : 
			return 1;
		break;

	}	
}