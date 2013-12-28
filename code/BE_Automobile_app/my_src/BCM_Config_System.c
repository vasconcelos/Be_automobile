/***************************************************************
 * BE Electronique Automobile          						   *
 * Created by Joao VASCONCELOS	 							   *
 * Date: 17/12/2013				 							   *
 *								 							   *
 * File: BCM_Config_System.c			 					   *
 * Description: This file contains all BCM system			   *
 * 				configurations  							   *
 ***************************************************************/
 
 
/***************************************************************
 *
 * INCLUDE FILES      		
 *						   		    
 ***************************************************************/
 #include "BCM_Config_System.h"
 
 
 
/***************************************************************
 *
 * ENUM TYPES     		
 *						   		    
 ***************************************************************/
 enum
 {
 	NORMAL_MODE,
 	DEGRADED_MODE,
 	LOW_POWER_MODE
 };// Run mode state
 
 enum
 {
 	PS_SUPERVISOR_EXCEPTION,
 	CAN_EXCEPTION,
 	POWER_SWITCH_EXCEPTION
 };// Set Exception (SW)
 

/***************************************************************
 *
 * GLOBAL VARIABLES     		
 *						   		    
 ***************************************************************/
 uint32_t mode_state = NORMAL_MODE;
 uint32_t LED_state = 0;
 
 
 
/***************************************************************
 *
 * CONFIGURE AND BOOT BCM SYSTEM FUNCTIONS
 *     								   	
 ***************************************************************/
 
 
 
 
/***************************************************************
 * 1 - void config_modes_and_clk(void):
 *
 * Description: Configure all used modes, the clock for each 
 * 				each mode, all peripheral used and the wake up
 *			    unit.  
 *
 * Return: void   								   	
 ****************************************************************/
 void config_modes_and_clk(void)
 {
 	// Enabling modes (Enable RUN0, RUN1, STOP0, RESET, SAFE, DRUN)
 	ME.MER.R = 0x0000043D;
 	
 	// Config for of all enable modes 
 	ME.RUN[0].R = 0x001F0074; // PLLON enable, FXOSCON enable, SYSCLK = FMPLL (Normal mode)
 	ME.RUN[1].R = 0x001F0022; // Clock fourni par le OSC externe (8MHz) (Mode dégradé)
 	ME.STOP0.B.PDO=1;     	  // Pad power sequence driver disabled
 	ME.STOP0.B.MVRON=0;   	  // Main Voltage regulator is off
 	ME.STOP0.B.DFLAON=1;  	  // Data flash is in power-down mode
 	ME.STOP0.B.CFLAON=1;  	  // Code flash is in power-down mode
	ME.STOP0.B.FMPLLON=0; 	  // FMPLL off (default status during STOP)
	ME.STOP0.B.FIRCON=0;  	  // Fast internal RC oscillator off
	ME.STOP0.B.FXOSC0ON=1;    // Fast external crystal oscillator off
	ME.STOP0.B.SYSCLK=0xF;	  // System clock is disabled
 	
 	// Config for all clock sources
 	// Clock out
 	CGM.OCDS_SC.R = 0x02000000; 	// Select FMPLL and divide by 1
 	CGM.OC_EN.B.EN = 1; 			// Write 1 to enable bit
 	// Clock system
 	CGM.FXOSC_CTL.R = 0x00800000; 	// keep reset settings
	CGM.SC_DC[0].R = 0x00; 			// no division for peripheral set 1
 	CGM.SC_DC[1].R = 0x00; 			// no division for peripheral set 2
 	CGM.SC_DC[2].R = 0x00; 			// no division for peripheral set 3
 	// FMPLL
 	CGM.FMPLL_CR.B.IDF = 1; 	  	// IDF[3:0] = IDF-1 = 2-1 = 1
	CGM.FMPLL_CR.B.ODF = 1;		  	// ODF[1:0] = log2(4)-1 = 2-1 = 1
	CGM.FMPLL_CR.B.NDIV = 0x40;   	// NDIV = 64
	CGM.FMPLL_CR.B.EN_PLL_SW = 1; 	// progressive transition
	// FM Modulation
	CGM.FMPLL_MR.B.MOD_PERIOD = 20; // fmod=50kHz
	CGM.FMPLL_MR.B.INC_STEP = 419;  // md = 2%
	CGM.FMPLL_MR.B.FM_EN = 1; 		// enable FM modulation
 	
 	 
 	// Config for behaviors to be used
 	ME.RUNPC[1].R = 0x00000030; 	// Running in RUN0 and RUN1 (Normal mode and degraded mode)
 	ME.LPPC[1].R = 0x00000400;  	// Running only in STOP0 (Low power consuption mode)
 
 
	// Configuring the peripherals
	// (Normal and Degraded mode) 
 	ME.PCTL[5].B.RUN_CFG = 0b001;  // Activating DSPI_n 1 for RUNPC[1]
 	ME.PCTL[17].B.RUN_CFG = 0b001; // Activating FlexCAN_n 1 for RUNPC[1]
 	ME.PCTL[32].B.RUN_CFG = 0b001; // Activating ADC for RUNPC[1]
 	ME.PCTL[57].B.RUN_CFG = 0b001; // Activating CTU for RUNPC[1]
 	ME.PCTL[60].B.RUN_CFG = 0b001; // Activating CANS for RUNPC[1]
 	ME.PCTL[68].B.RUN_CFG = 0b001; // Activating SIUL 1 for RUNPC[1]
 	//ME.PCTL[69].B.RUN_CFG = 0b001; // Activating WKUP for RUNPC[1]
 	ME.PCTL[72].B.RUN_CFG = 0b001; // Activating eMIOS_n 0 for RUNPC[1]
 	ME.PCTL[73].B.RUN_CFG = 0b001; // Activating eMIOS_n 1 for RUNPC[1]
 	//ME.PCTL[91].B.RUN_CFG = 0b001; // Activating RTC/API for RUNPC[1]
 	ME.PCTL[92].B.RUN_CFG = 0b001; // Activating PIT for RUNPC[1]
 	// (Low power mode)
 	ME.PCTL[69].B.LP_CFG = 0b001;  // Activating WKUP for LPPC[1]
 	ME.PCTL[68].B.LP_CFG = 0b001;  // Activating SIUL 1 for LPPC[1]
	//ME.PCTL[91].B.LP_CFG = 0b001; // Activating RTC/API for LPPC[1]
 	
 	enable_Normal_Mode(); // Normal mode enable (RUN0)
 	
 	// Config for WakeUp unit
 	WKUP.WIPUER.R = 0x000FFFFF; 	// Enable pullups on all 18 wakeup pins
 	WKUP.WISR.R = 0x00000040;   	// Clear WKUP[6]flag
 	WKUP.WRER.R = 0x00000040;   	// Enable WKUP[6]for wakeup event
 	WKUP.WIREER.R = 0x00000040; 	// Enable Rising Edge of WKUP[6]
 	WKUP.WIFEER.R = 0x00000040; 	// Enable Falling Edge of WKUP[6]
 	WKUP.WIFER.R = 0x00000040; 	 	// Enable Filter of WKUP[6]
 	//INTC_InstallINTCInterruptHandler(WKUP_Interrupt,46,9);
 	
 	// Watchdog configurations
 	SWT.SR.R = 0x0000c520; // Write keys to clear soft lock bit
	SWT.SR.R = 0x0000d928;
	SWT.TO.R = 0x00000C80; // Timeout period = 3200 (25 ms)
	SWT.CR.R = 0x8000015E; // Disable Watchdog and other features (see pag 790 of Reference_Manual_rev7)
 }	
 
 
 
/***************************************************************
 * 2 - void enable_Normal_mode():
 *
 * Description: Enable the RUN0 mode (normal mode)
 *
 * Return: void								   	
 ****************************************************************/
void enable_Normal_Mode(void)
{
	//Mode Transition to enter RUN0 mode
    ME.MCTL.R = 0x40005AF0; // Enter RUN0 Mode & Key
  	ME.MCTL.R = 0x4000A50F; // Enter RUN0 Mode & Inverted Key
  									
  
	while (ME.GS.B.S_MTRANS) {}  		 // Wait for mode transition to complete   	 
	while(ME.GS.B.S_CURRENTMODE != 4) {} // Verify RUN0 is the current mode 
	
	mode_state = NORMAL_MODE;			 // Updating mode state
	PIT_change_Timeout();				 // Changing counter value of the PIT
}


/***************************************************************
 * 3 - void enable_Degraded_Mode():
 *
 * Description: Enable the RUN1 mode (degraded mode)
 *
 * Return: void								   	
 ****************************************************************/
void enable_Degraded_Mode(void)
{
	//Mode Transition to enter RUN1 mode
    ME.MCTL.R = 0x50005AF0;  // Enter RUN1 Mode & Key
  	ME.MCTL.R = 0x5000A50F;  // Enter RUN1 Mode & Inverted Key
  									
  
	while (ME.GS.B.S_MTRANS) {}  		 // Wait for mode transition to complete   
	while(ME.GS.B.S_CURRENTMODE != 5) {} // Verify RUN1 is the current mode
	
	mode_state = DEGRADED_MODE;			 // Updating mode state
	PIT_change_Timeout();				 // Changing counter value of the PIT	
}



/***************************************************************
 * 4 - void enable_Low_Power_mode():
 *
 * Description: Enable the STOP0 mode (low power mode)
 *
 * Return: void								   	
 ****************************************************************/
void enable_Low_Power_mode(void)
{

	mode_state = LOW_POWER_MODE;
	
	//Mode Transition to enter RUN1 mode
    ME.MCTL.R = 0xA0005AF0;  // Enter RUN1 Mode & Key
  	ME.MCTL.R = 0xA000A50F;  // Enter RUN1 Mode & Inverted Key
  									
	while (ME.GS.B.S_MTRANS) {}  			  // Wait for mode transition to complete   	 
	while(ME.GS.B.S_CURRENTMODE != 0xA) {}    // Verify RUN1 is the current mode
		
	if(mode_state == NORMAL_MODE)
	{
		while(ME.GS.B.S_CURRENTMODE != 4) {}  // Verify RUN0 is still current mode
	}
	else
	{
		while(ME.GS.B.S_CURRENTMODE != 5) {}  // Verify RUN1 is still current mode
	}
	WKUP.WISR.R = 0x00000040; 				  // MPC56xxB: Clear wake up flag	
}



/***************************************************************
 * 5 - void enableIrq()  
 *
 * Description: Authorization of maskables interruptions 
 *
 * Return: void							   	
 ****************************************************************/
void enableIrq(void) {
  INTC.CPR.B.PRI = 0;          /* Single Core: Lower INTC's current priority */
  asm(" wrteei 1");	    	   /* Enable external interrupts */
}



/***************************************************************
 * 6 - void config_PORT_E()	
 *
 * Description: Config for all push buttons and leds for tests
 *
 * Return: void						   	
 ****************************************************************/
void config_PORT_E(void) 
{
	// Push button
	//configurer_GPIO(64,BINARY_INPUT);
	SIU.PCR[64].R = 0x0103; 			// WKUP Imput PE[0] = WKUP[6]
  	configurer_GPIO(65,BINARY_INPUT);
  	configurer_GPIO(66,BINARY_INPUT);
  	configurer_GPIO(67,BINARY_INPUT);
	// LEDS
  	configurer_GPIO(68,PUSH_PULL_OUTPUT);
  	configurer_GPIO(69,PUSH_PULL_OUTPUT);
  	configurer_GPIO(70,PUSH_PULL_OUTPUT);
  	configurer_GPIO(71,PUSH_PULL_OUTPUT);
}


/***************************************************************
 * 7 - void initPIT()  	
 *
 * Description: Config for all periodic interruption timers
 *
 * Return: void							   	
 ****************************************************************/
void initPIT(void)
{
  // Config for periodic timer interruption for the light manager system
  PIT.PITMCR.R = 0x00000001;        // Enable PIT and configure to stop in debug mode 
  PIT.CH[0].LDVAL.R = 640000;       // Timeout= 640000 sysclks x 1sec/64M sysclks = 10 ms
  PIT.CH[0].TCTRL.R = 0x000000003;  // Enable PIT0 interrupt & start PIT counting
  
  // Configuring periodic timer for the low power manager system
  PIT.PITMCR.R = 0x00000001;        // Enable PIT and configure to stop in debug mode 
  PIT.CH[1].LDVAL.R = 64000000;     // Timeout= 64000000 sysclks x 1sec/64M sysclks = 1 s
  PIT.CH[1].TCTRL.R = 0x000000003;  //  Enable PIT1 interrupt & start PIT counting
}


/***************************************************************
 * 8 - void PIT_change_Timeout()  	
 *
 * Description: This function changes the counter value when the
 *				run mode must be changed							   	
 ****************************************************************/
 void PIT_change_Timeout(void)
 {
 	if(mode_state == NORMAL_MODE)
 	{
 		PIT.CH[0].LDVAL.R = 640000;   // 10 ms
 		PIT.CH[1].LDVAL.R = 64000000; // 1 s
 	}
 	else
 	{
 		PIT.CH[0].LDVAL.R = 80000;    // 10 ms
 		PIT.CH[1].LDVAL.R = 8000000;  // 1 s
 	}
 }
 
 
 void WKUP_Interrupt(void)
 {
 	enable_Normal_Mode();
 	PIT_change_Timeout();
 }
 
 
/***************************************************************
 * 9 - void disableWatchdog():
 *
 * Description: Disable the internal watchdog
 *
 * Return: void								   	
 ****************************************************************/
 void disableWatchdog(void) 
 {
	SWT.SR.R = 0x0000c520;  // Write keys to clear soft lock bit
	SWT.SR.R = 0x0000d928;
	SWT.CR.B.WEN = 0; 		// watchdog disable (WEN)
	SWT.CR.B.SLK = 1;
 }


// todo
/***************************************************************
 * 10 - void enableWatchdog():
 *
 * Description: Enable the internal watchdog
 *
 * Return: void								   	
 ****************************************************************/
void enableWatchdog(void)
{
	SWT.SR.R = 0x0000c520;  // Write keys to clear soft lock bit
	SWT.SR.R = 0x0000d928;
	SWT.CR.B.WEN = 1; 		// watchdog enable (WEN)
	SWT.CR.B.SLK = 1; 
}



/***************************************************************
 * 10 - void set_Exception():
 *
 * Description: This function is used to set a SW interruption
 *
 * Param exception_Type: The type of the exception 
 *
 * Return: void								   	
 ****************************************************************/
void set_Exception(uint32_t exception_Type) 
{
	switch(exception_Type)
	{
		case PS_SUPERVISOR_EXCEPTION:
			INTC.SSCIR[0].B.SET = 1;
		break;
		
		case CAN_EXCEPTION:
			INTC.SSCIR[1].B.SET = 1;
		break;
		
		case POWER_SWITCH_EXCEPTION:
			INTC.SSCIR[2].B.SET = 1;
		break;
		
		default:
		break;
	}
}