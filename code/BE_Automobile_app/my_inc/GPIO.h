#ifndef _GPIO_H_
#define _GPIO_H_

enum
{
	BINARY_INPUT,ANALOG_INPUT,PUSH_PULL_OUTPUT,OPEN_DRAIN_OUTPUT
};


int configurer_GPIO(int indice_PCR_register,int mode);


#endif