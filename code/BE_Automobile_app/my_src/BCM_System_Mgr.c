/***************************************************************
 * BE Electronique Automobile          						   *
 * Created by Joao VASCONCELOS	 							   *
 * Date: 17/12/2013				 							   *
 *								 							   *
 * File: BCM_System_Mgr.c			 					   	   *
 * Description: This file contains the main application of     *
 *				the BCM system and all call_backs.			   *
 ***************************************************************/
 
 
/***************************************************************
 *															   *
 * INCLUDE FILES      								   		   *
 *															   *
 ***************************************************************/
 #include "BCM_System_Mgr.h"
 

/***************************************************************
 *															   *
 * DEFINES	     								   		       *
 *															   *
 ***************************************************************/
 
 // CAN messages
 #define BCM_ADRESS 0x00000;
 #define LCM_ADRESS 0x10000;
 #define POWER_SUPPLY_ERRORS_TOPIC 0x010;
 #define POWER_SWITCH_ERRORS_TOPIC 0x020;
 #define LIGHTS_TOPIC 0x030;
 #define CAN_ERRORS_TOPIC 0x040;
 
 
 
/***************************************************************
 *															   *
 * ENUM TYPES	     								   	       *
 *															   *
 ***************************************************************/ 
 enum
 {
 	FEUX_OFF,
 	FEUX_DE_PRESENCE,
 	FEUX_DE_ROUTE,
 	FEUX_MAX,
 	CLIGNOTANT_DROIT_DISABLE,
 	CLIGNOTANT_DROIT_ENABLE,
 	CLIGNOTANT_GAUCHE_DISABLE,
 	CLIGNOTANT_GAUCHE_ENABLE
 };// Phare state
 
 enum
 {
 	BUTTON_PHARES_DISABLE,
 	BUTTON_PHARES_ENABLE,
 	BUTTON_CLIGN_DROIT_DISABLE,
 	BUTTON_CLIGN_DROIT_ENABLE,
 	BUTTON_CLIGN_GAUCHE_DISABLE,
 	BUTTON_CLIGN_GAUCHE_ENABLE
 };// Push buttons state
 
 enum
 {
 	SENSOR_OFF,
 	SENSOR_ON
 };// Sensor state
 
 enum
 {
 	LIGHTS_MANUAL_MODE_OFF,
 	LIGHTS_MANUAL_MODE_ON
 };// Manual mode
 
 enum
 {
 	SWITCH_OFF_PHARES,
 	SWITCH_ON_FEUX_DE_PRESENCE,
 	SWITCH_ON_FEUX_DE_ROUTE,
 	SWITCH_ON_FEUX_MAX,
 	SWITCH_ON_CLIGN_DROIT,
 	SWITCH_OFF_CLIGN_DROIT,
 	SWITCH_ON_CLIGN_GAUCHE,
 	SWITCH_OFF_CLIGN_GAUCHE
 };// LIGHTS_TOPIC
 
 enum
 {
 	POWER_SWITCH_PROBLEM
 };// POWER_SWITCH_ERRORS_TOPIC
 
 enum
 {
 	POWER_SUPPLY_PROBLEM,
 	CHANGE_TO_DEGRADED_MODE
 };// POWER_SUPPLY_ERRORS_TOPIC
 
 
/***************************************************************
 *															   *
 * GLOBAL VARIABLES	     						   	     	   *
 *															   *
 ***************************************************************/
 uint32_t phare_state = FEUX_OFF;
 uint32_t clign_gauche_state = CLIGNOTANT_GAUCHE_DISABLE;
 uint32_t clin_droit_state = CLIGNOTANT_DROIT_DISABLE;
 uint32_t button_phare_state = BUTTON_PHARES_DISABLE;
 uint32_t button_clign_gauche_state = BUTTON_CLIGN_GAUCHE_DISABLE;
 uint32_t button_clign_droit_state = BUTTON_CLIGN_DROIT_DISABLE;
 uint32_t sensor_state = SENSOR_OFF;
 uint32_t manual_mode = LIGHTS_MANUAL_MODE_OFF;
 uint32_t previous_phare_state = FEUX_OFF;
 uint32_t CAN_Sleep_Counter = 0;
 uint32_t LP_Counter = 0;
 
 

/***************************************************************
 *															   *
 * LIGHT MANAGER FUNCTIONS	     						   	   *
 *															   *
 ***************************************************************/
 
 
/***************************************************************
 * 1 - void init_Light_Manager(void):
 *
 * Description: Initialize the BCM light manager system  
 *
 * Return: void   								   	
 ****************************************************************/
 void init_Light_Manager(void)
 {
  // 59 = number of the interrupt vector 
  // 2 = priority level.
  INTC_InstallINTCInterruptHandler(PIT0_Interrupt,59,5);
	
  SIU.PGPDO[2].R = 0x0F000000;
 }
 
/***************************************************************
 * 2 - void PIT0_Interrupt(void):
 *
 * Description: PIT0 call back for read the state of the push 
 *				button and light sensor (every 10 ms)
 *
 * Return: void   								   	
 ****************************************************************/
 void PIT0_Interrupt(void)
 {
 	//Cleannig the PIT0 flag
	PIT.CH[0].TFLG.B.TIF = 1;
	
	// Reading the push button for the "phares"
	if(!SIU.GPDI[64].B.PDI) // if button PE0 is enable
	{
		LP_Counter = 0;
		CAN_Sleep_Counter = 0;
		
		if(button_phare_state == BUTTON_PHARES_DISABLE)
		{	
			button_phare_state = BUTTON_PHARES_ENABLE;
			switch (phare_state)
			{
				case FEUX_OFF:
					// send can message here to switch on the feux de presence
					phare_state = FEUX_DE_PRESENCE;
					manual_mode = LIGHTS_MANUAL_MODE_ON;
					SIU.GPDO[69].B.PDO = 0;
				break;
				
				case FEUX_DE_PRESENCE:
					// send can message here to switch on the feux de route + feux de presence
					phare_state = FEUX_DE_ROUTE;
					SIU.GPDO[68].B.PDO = 0;
					SIU.GPDO[69].B.PDO = 1;	
				break;
				
				case FEUX_DE_ROUTE:
					// send can message here to switch on the feux max
					phare_state = FEUX_MAX;
					SIU.GPDO[69].B.PDO = 0;
				break;
				
				case FEUX_MAX:
					if(sensor_state == SENSOR_ON)
					{
						manual_mode = LIGHTS_MANUAL_MODE_ON;
						phare_state = FEUX_DE_ROUTE;
						SIU.GPDO[68].B.PDO = 0;
						SIU.GPDO[69].B.PDO = 1;
						// send CAN message to switch on the feux de route + presence
					}
					else
					{
						// send can message here to switch off the phares
						manual_mode = LIGHTS_MANUAL_MODE_OFF;
						phare_state = FEUX_OFF;
						SIU.GPDO[68].B.PDO = 1;
						SIU.GPDO[69].B.PDO = 1;	
					}
				break;
				
				default:
				break;
			}
		}
	}
	else
	{
		button_phare_state = BUTTON_PHARES_DISABLE;
	}
	
	
	// Reading the push button for the "clignotant droit"
	if(!SIU.GPDI[65].B.PDI) // if button PE1 is enable
	{
		LP_Counter = 0;
		CAN_Sleep_Counter = 0;
		
		if(button_clign_droit_state == BUTTON_CLIGN_DROIT_DISABLE)
		{	
			button_clign_droit_state = BUTTON_CLIGN_DROIT_ENABLE;
			switch (clin_droit_state)
			{
				case CLIGNOTANT_DROIT_DISABLE:
					// send can message here
					clin_droit_state = CLIGNOTANT_DROIT_ENABLE;
					SIU.GPDO[70].B.PDO = 0;
				break;
				
				case CLIGNOTANT_DROIT_ENABLE:
					// send can message here
					clin_droit_state = CLIGNOTANT_DROIT_DISABLE;
					SIU.GPDO[70].B.PDO = 1;
				break;
				
				default:
				break;
			}
		}
	}
	else
	{
		button_clign_droit_state = BUTTON_CLIGN_DROIT_DISABLE;
	}
	
	// Reading the push button for the "clignotant gauche"
	if(!SIU.GPDI[66].B.PDI) // if button PE2 is enable
	{
		LP_Counter = 0;
		CAN_Sleep_Counter = 0;
		
		if(button_clign_gauche_state == BUTTON_CLIGN_GAUCHE_DISABLE)
		{	
			button_clign_gauche_state = BUTTON_CLIGN_GAUCHE_ENABLE;
			switch (clign_gauche_state)
			{
				case CLIGNOTANT_GAUCHE_DISABLE:
					// send can message here
					clign_gauche_state = CLIGNOTANT_GAUCHE_ENABLE;
					SIU.GPDO[71].B.PDO = 0;
				break;
				
				case CLIGNOTANT_GAUCHE_ENABLE:
					// send can message here
					clign_gauche_state = CLIGNOTANT_GAUCHE_DISABLE;
					SIU.GPDO[71].B.PDO = 1;
				break;
				
				default:
				break;
			}
		}
	}
	else
	{
		button_clign_gauche_state = BUTTON_CLIGN_GAUCHE_DISABLE;
	}

	// Reading light sensor (todo)
/*	if()
	{
		if(manual_mode == LIGHTS_MANUAL_MODE_ON)
		{
			if(phare_state == FEUX_DE_PRESENCE) 
			{	
				LP_Counter = 0;
				CAN_Sleep_Counter = 0;
				
				previous_phare_state = phare_state;
				phare_state = FEUX_DE_ROUTE;
				SIU.GPDO[68].B.PDO = 0;
				SIU.GPDO[69].B.PDO = 1;
				// send  CAN message here to switch on the feux de présence et feux de route
			}		
		}
		else
		{
			if(phare_state == FEUX_OFF)
			{
				LP_Counter = 0;
				CAN_Sleep_Counter = 0;
				
				phare_state = FEUX_DE_ROUTE;
				SIU.GPDO[68].B.PDO = 0;
				SIU.GPDO[69].B.PDO = 1;
				// send  CAN message here to switch on the feux de présence et feux de route
			}
		}
		sensor_state = SENSOR_ON;
	}
	else
	{
		if(manual_mode == LIGHTS_MANUAL_MODE_OFF)
		{
			if(phare_state != FEUX_OFF)
			{
				LP_Counter = 0;
				CAN_Sleep_Counter = 0;
				
				phare_state = FEUX_OFF;
				SIU.GPDO[68].B.PDO = 1;
				SIU.GPDO[69].B.PDO = 1;
				// send CAN message here to switch of the phares
			}
		}
		else
		{
			if((previous_phare_state == FEUX_DE_PRESENCE) && (phare_state != FEUX_MAX))
			{
				LP_Counter = 0;
				CAN_Sleep_Counter = 0;
			
				// send can message here to switch on the feux de presence
				phare_state = FEUX_DE_PRESENCE;
				SIU.GPDO[68].B.PDO = 1;
				SIU.GPDO[69].B.PDO = 0;
			}
		}
		sensor_state = SENSOR_OFF;
	}
*/


 }
 
 
 
 
 
/***************************************************************
 *															   *
 * POWER SUPPLY MANAGER FUNCTIONS	     				   	   *
 *															   *
 ***************************************************************/
 
/***************************************************************
 * 1 - void init_Supervisor_Mngr(void):
 *
 * Description: Initialize the BCM power supply supervisor
 *				manager system  
 *
 * Return: void   								   	
 ****************************************************************/
 void init_Supervisor_Mngr(void)
 {
 		// init SW0 interruption
  	INTC_InstallINTCInterruptHandler(SW0_Interrupt,0,4);
 }


/***************************************************************
 * 2 - void SW0_Interrupt(void):
 *
 * Description: SW0 call back for read the state of the power 
 *				supply supervisor and take decisions.
 *				This function is called by PS Supervisor SW
 *
 * Return: void   								   	
 ****************************************************************/
 void SW0_Interrupt(void)
 {
	// Clear flag
	INTC.SSCIR[0].B.CLR = 1;
	// Clear Low power counter
	LP_Counter = 0;
	
	// TODO: place the code here
	
	// Remise du compteur à 0;
		
		// Si Vsense < 8.6V ou Vsup < 6V (appel par SW interrupt)
		    	// vérification de Vbat (si faibe allumer LED faible batterie)
		
		// Si 3.5<Vdd<4.5 (appel par SW interrupt)
				// Allumage led de batterie faible
				// Envoi de message CAN au LCM
				// Enable internal watchdog (50 ms)
				// Passage en mode dégradé
				
		// Si Vsup < 4V // (appel par SW)
				// Se préparer pour un reset
		
		// Si problème sur l'alim de l'interface CAN
				// Passage de l'interface CAN en mode sleep jusqu'à ce que le pb disparaisse
					//(On cherchera à faire revenir le CAN)
					
		// Si Vdd est bon
			// Passage à normal mode
 }
 
 
/***************************************************************
 *															   *
 * CAN MESSAGES MANAGER	FUNCTIONS	   				   	   	   *
 *															   *
 ***************************************************************/
 
/***************************************************************
 * 1 - void init_CAN_Messages_Mgr(void):
 *
 * Description: Initialize the BCM CAN messages manager system  
 *
 * Return: void   								   	
 ****************************************************************/
 void init_CAN_Messages_Mgr(void)
 {
  	// init SW1 interruption
  	INTC_InstallINTCInterruptHandler(SW1_Interrupt,1,3);
 }


/***************************************************************
 * 2 - void SW1_Interrupt(void):
 *
 * Description: SW1 call back for read all CAN received messages
 *				and take decisions.
 *				This function is called by CAN SW Exception
 *
 * Return: void   								   	
 ****************************************************************/
 void SW1_Interrupt(void)
 {
	// Clear flag
	INTC.SSCIR[1].B.CLR = 1;
	// Clear Low power counter
	LP_Counter = 0;
	CAN_Sleep_Counter = 0;
	
	// TODO: place the code here
	
	// Remise des compteurs à 0
		
		// Problème sur le power switch (LCM)
			// Allumage de LEDS (LE LCM vérifie et coupe le courrant du phare/Power switch)
			// Etteidre la led si le pb disparait
			
		// Problème sur l'alim du LCM
			// Allumage led + passage du CAN à 125 kbauds
			
		// Bus off (à l'envoie d'un message CAN)
			//Remettre le bus en on
			//Si bus off 5 fois à la suite, abandoner le message à envoyer + allumage led
			
		// Si problème matériel
			// Notifier en allumant une led + passage du CAN en receive only
			// Chercher la panne et chercher à faire revenir le CAN en mode normal
			
		// Si autre problème sur le LCM
			// allumage LED
 }
 
 
/***************************************************************
 *															   *
 * LOW POWER MANAGER FUNCTIONS	     				   	   	   *
 *															   *
 ***************************************************************/
 
/***************************************************************
 * 1 - void init_Low_Power_Mgr(void):
 *
 * Description: Initialize the BCM Low power manager system  
 *
 * Return: void   								   	
 ****************************************************************/
 void init_Low_Power_Mgr(void)
 {
 	INTC_InstallINTCInterruptHandler(PIT1_Interrupt,60,8);
 }

/***************************************************************
 * 2 - void PIT1_Interrupt(void):
 *
 * Description: This function (call back) is called every second.
 *				The BCM is changed to the STOP0 mode if the 
 *				LP_Counter reach 10 sec.
 *				The CAN interface is changed to the sleep mode 
 *				if CAN_Sleep_Counter reach 2 sec.
 *
 * Return: void   								   	
 ****************************************************************/
 void PIT1_Interrupt(void)
 {
 	//Cleannig the PIT1 flag
	PIT.CH[1].TFLG.B.TIF = 1;

	CAN_Sleep_Counter++;
	LP_Counter++;
	
	if(CAN_Sleep_Counter == 2)
	{
		CAN_Sleep_Counter = 0;
		// put CAN Interface in sleep mode
	}
	
	if(LP_Counter == 10)
	{
		CAN_Sleep_Counter = 0;
		LP_Counter =0;
		// put CAN Interface in sleep mode
		// do other low power configs + desactiver les ADC's (voir doc)
		//enable_Low_Power_mode();
	}
 }
 
 
/***************************************************************
 *															   *
 * WATCHDOG MANAGER SYSTEM (FOR DEGRADED MODE)  	   	   	   *
 *															   *
 ***************************************************************/
 
/***************************************************************
 * 1 - void init_Low_Power_Mgr(void):
 *
 * Description: Initialize the BCM Watchdog manager system  
 *
 * Return: void   								   	
 ****************************************************************/
 void init_Watchdog_Manager_Mgr(void)
 {
 	INTC_InstallINTCInterruptHandler(SWT_Interrupt,28,2);
 }

/***************************************************************
 * 2 - void SWT_Interrupt(void):
 *
 * Description: This function charges the timeout period.
 *				If the timeout period is not loaded before the
 *				second interruption, a reset is perfomed
 *
 * Return: void   								   	
 ****************************************************************/
 void SWT_Interrupt(void)
 {
 	SWT.SR.R = 0x0000A602;  // Watchdog service key
	SWT.SR.R = 0x0000B480;
	
	SWT.SR.R = 0x0000c520;  // Write keys to clear soft lock bit
	SWT.SR.R = 0x0000d928;
	SWT.IR.B.TIF = 1;
	SWT.CR.B.SLK = 1;
	
	
 }