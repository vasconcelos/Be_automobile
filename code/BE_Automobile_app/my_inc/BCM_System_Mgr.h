/***************************************************************
 * BE Electronique Automobile          						   *
 * Created by Joao VASCONCELOS	 							   *
 * Date: 17/12/2013				 							   *
 *								 							   *
 * File: BCM_System_Mgr.h			 					   	   *
 * Description: This file contains the main application of     *
				the BCM system and all call_backs.			   *
 ***************************************************************/
 
 
#ifndef _BCM_SYSTEM_MGR_H_
#define _BCM_SYSTEM_MGR_H_
 
 
/***************************************************************
 *															   *		
 * INCLUDE FILES      								   		   *
 *															   *
 ***************************************************************/
 #include "BCM_Config_System.h"
 
 
 
/***************************************************************
 *															   *
 * PROTOTYPES         								   		   *
 *															   * 
 ***************************************************************/
 
 // init functions
 void init_Light_Manager(void);
 void init_Supervisor_Mngr(void);
 void init_CAN_Messages_Mgr(void);
 void init_Low_Power_Mgr(void);
 void init_Watchdog_Manager_Mgr(void);
 
 // call back functions
 void PIT0_Interrupt(void);
 void PIT1_Interrupt(void);
 void SW0_Interrupt(void);
 void SW1_Interrupt(void);
 void SWT_Interrupt(void);
 

 
#endif//_BCM_SYSTEM_MGR_H_