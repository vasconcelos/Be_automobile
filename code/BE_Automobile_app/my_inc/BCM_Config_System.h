/***************************************************************
 * BE Electronique Automobile          						   *
 * Created by Joao VASCONCELOS	 							   *
 * Date: 17/12/2013				 							   *
 *								 							   *
 * File: BCM_Config_System.h			 					   *
 * Description: This file contains all BCM system			   *
 * 				configurations   							   *
 ***************************************************************/
 

#ifndef _BCM_CONFIG_SYSTEM_H_
#define _BCM_CONFIG_SYSTEM_H_

/***************************************************************
 * INCLUDE FILES      								   		    *
 ***************************************************************/
 #include "MPC5604B_M27V.h"
 #include "GPIO.h"
 

 
/***************************************************************
 * PROTOTYPES         								   		    *
 ***************************************************************/
 void config_modes_and_clk(void);
 void enable_Normal_Mode(void);
 void enable_Degraded_Mode(void);
 void enable_Low_Power_mode(void);
 void disableWatchdog(void);
 void config_PORT_E(void);
 void enableIrq(void);
 void initPIT(void);
 void PIT_change_Timeout(void);
 void enableWatchdog(void) ;
 void set_Exception(uint32_t exception_Type);
 
#endif//_BCM_CONFIG_SYSTEM_H_