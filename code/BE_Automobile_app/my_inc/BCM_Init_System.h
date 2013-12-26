/***************************************************************
 * BE Electronique Automobile          						   *
 * Created by Joao VASCONCELOS	 							   *
 * Date: 17/12/2013				 							   *
 *								 							   *
 * File: Init_System.h			 							   *
 * Description: This file contains all system configurations   *
 ***************************************************************/
 
 
 
  /***************************************************************
 * INCLUDE FILES      								   		    *
 ***************************************************************/
 #include "MPC5604B_M27V.h"
 
 
 /***************************************************************
 * PROTOTYPES         								   		    *
 ***************************************************************/
 void config_modes_and_clk(void);
 void enable_Normal_Mode(void);
 void enable_Degraded_Mode(void);
 void disableWatchdog(void);
 
 
 
 
 // test
 void config_PORT_E(void);
 void PIT1_Interrupt(void);
 void enableIrq(void);
 void initPIT(void);
 

 
 
 