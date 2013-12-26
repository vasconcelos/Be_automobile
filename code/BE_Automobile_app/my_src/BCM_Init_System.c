/***************************************************************
 * BE Electronique Automobile          						   *
 * Created by Joao VASCONCELOS	 							   *
 * Date: 17/12/2013				 							   *
 *								 							   *
 * File: Init_System.c			 							   *
 * Description: This file contains all system configurations   *
 ***************************************************************/
 
 
/***************************************************************
 *
 * INCLUDE FILES      		
 *						   		    
 ***************************************************************/
 #include "BCM_Init_System.h"
 
 uint32_t LED_state = 0;
 
 
 
 

/***************************************************************
 *
 * BCM APP FUNCTIONS   
 *     								   	
 ***************************************************************/
 
 
 
/***************************************************************
 * config_modes_and_clk      								   	
 ****************************************************************/
 void config_modes_and_clk(void)
 {
 	// Enabling modes (Enable RUN0, RUN1, STOP0, RESET, SAFE, DRUN)
 	ME.MER.R = 0x0000043D;
 	
 	// Configuring the clock souces
 	ME.RUN[0].R = 0x001F0074; // PLLON enable, FXOSCON enable, SYSCLK = FMPLL
 	ME.RUN[1].R = 0x001F0022; // Clock fourni par le OSC externe (8MHz) (Mode dégradé)
 	ME.STOP0.B.FIRCON = 0;
 	ME.STOP0.B.FXOSC0ON = 1;
 	ME.STOP0.B.SYSCLK = 0x2;
 	
 	
 	// Configuring clock sources for each mode
 	// 1 - Clock out
 	CGM.OCDS_SC.R = 0x02000000; // Select FMPLL and divide by 1
 	CGM.OC_EN.B.EN = 1; // Write 1 to enable bit
 	// 2 - Clock system
 	CGM.FXOSC_CTL.R = 0x00800000; // keep reset settings
	CGM.SC_DC[0].R = 0x00; // no division for peripheral set 1
 	CGM.SC_DC[1].R = 0x00; // no division for peripheral set 2
 	CGM.SC_DC[2].R = 0x00; // no division for peripheral set 3
 	// 3 - FMPLL
 	CGM.FMPLL_CR.B.IDF = 1; 	  // IDF[3:0] = IDF-1 = 2-1 = 1
	CGM.FMPLL_CR.B.ODF = 1;		  // ODF[1:0] = log2(4)-1 = 2-1 = 1
	CGM.FMPLL_CR.B.NDIV = 0x40;   // NDIV = 64
	CGM.FMPLL_CR.B.EN_PLL_SW = 1; //progressive transition
	// 4 - FM Modulation
	CGM.FMPLL_MR.B.MOD_PERIOD = 20; /* fmod=50kHz */
	CGM.FMPLL_MR.B.INC_STEP = 419; /* md = 2% */
	CGM.FMPLL_MR.B.FM_EN = 1; /* enable FM */
 	
 	 
 	// Configuring the modes to be used
 	ME.RUNPC[1].R = 0x00000030; // Running in RUN0 and RUN1 (Normal mode and degraded mode)
 	ME.LPPC[1].R = 0x00000400;  // Running only in STOP0 (Low power consuption mode)
 
 
	// Configuring the peripherals
	// 1 - Normal and Degraded mode 
 	ME.PCTL[5].B.RUN_CFG = 0b001; // Activating DSPI_n 1 for RUNPC[1]
 	ME.PCTL[17].B.RUN_CFG = 0b001; // Activating FlexCAN_n 1 for RUNPC[1]
 	ME.PCTL[32].B.RUN_CFG = 0b001; // Activating ADC for RUNPC[1]
 	ME.PCTL[57].B.RUN_CFG = 0b001; // Activating CTU for RUNPC[1]
 	ME.PCTL[60].B.RUN_CFG = 0b001; // Activating CANS for RUNPC[1]
 	ME.PCTL[68].B.RUN_CFG = 0b001; // Activating SIUL 1 for RUNPC[1]
 	ME.PCTL[69].B.RUN_CFG = 0b001; // Activating WKUP for RUNPC[1]
 	ME.PCTL[72].B.RUN_CFG = 0b001; // Activating eMIOS_n 0 for RUNPC[1]
 	ME.PCTL[73].B.RUN_CFG = 0b001; // Activating eMIOS_n 1 for RUNPC[1]
 	ME.PCTL[91].B.RUN_CFG = 0b001; // Activating RTC/API for RUNPC[1]
 	ME.PCTL[92].B.RUN_CFG = 0b001; // Activating PIT for RUNPC[1]
 	// 2 - Low power mode
 	// white the code here 
 }
 
 
 
/***************************************************************
 * enable_Normal_mode      								   	
 ****************************************************************/
void enable_Normal_mode(void)
{
	//Mode Transition to enter RUN0 mode
    ME.MCTL.R = 0x40005AF0; // Enter RUN0 Mode & Key
  	ME.MCTL.R = 0x4000A50F; // Enter RUN0 Mode & Inverted Key
  									
  
	while (ME.GS.B.S_MTRANS) {}  // Wait for mode transition to complete   
	 
	while(ME.GS.B.S_CURRENTMODE != 4) {} // Verify RUN0 is the current mode 	
}


/***************************************************************
 * enable_Degraded_mode      								   	
 ****************************************************************/
void enable_Degraded_mode(void)
{
	//Mode Transition to enter RUN1 mode
    ME.MCTL.R = 0x50005AF0; // Enter RUN1 Mode & Key
  	ME.MCTL.R = 0x5000A50F; // Enter RUN1 Mode & Inverted Key
  									
  
	while (ME.GS.B.S_MTRANS) {}  // Wait for mode transition to complete   
	 
	while(ME.GS.B.S_CURRENTMODE != 5) {} // Verify RUN1 is the current mode 	
}



/***************************************************************
 * disableWatchdog      								   	
 ****************************************************************/
void disableWatchdog(void) 
{
	SWT.SR.R = 0x0000c520; /* Write keys to clear soft lock bit */
	SWT.SR.R = 0x0000d928;
	SWT.CR.R = 0x8000010A; /* Clear watchdog enable (WEN) */
}





/***************************************************************
 * Test functions   								   	
 ****************************************************************/
//configuration du port E en sortie, activation de la fonction GPIO.
void config_PORT_E(void) 
{
	SIU.PCR[68].R = 0x0200;
	SIU.PCR[69].R = 0x0200;
	SIU.PCR[70].R = 0x0200;
	SIU.PCR[71].R = 0x0200;	
}


//initialisation du timer PIT1
//celui s'incrémente au rythme de l'horloge système (1MHz). Pour avoir un débordement toutes les 500 ms,
//il faut fournir 500000 comme valeur au compteur.
void initPIT(void) {
  PIT.PITMCR.R = 0x00000001;       /* Enable PIT and configure to stop in debug mode */
  PIT.CH[1].LDVAL.R = 4000000;     /* Timeout= 500000 sysclks x 1sec/1M sysclks = 0.5 s */
  PIT.CH[1].TCTRL.R = 0x000000003; /* Enable PIT1 interrupt & start PIT counting */ 
}

//autorisation des interruptions masquables
void enableIrq(void) {
  INTC.CPR.B.PRI = 0;          /* Single Core: Lower INTC's current priority */
  asm(" wrteei 1");	    	   /* Enable external interrupts */
}



void PIT1_Interrupt(void) 
{
	//raz du flag interruption PIT1
	PIT.CH[1].TFLG.B.TIF = 1;
	
	//modif état des LED du port E
	if(LED_state == 0) 
	{
		LED_state = 1;
		SIU.PGPDO[2].R = 0x0A000000;  	
	}
	else 
	{
		LED_state = 0;
		SIU.PGPDO[2].R = 0x05000000;
	}	
}