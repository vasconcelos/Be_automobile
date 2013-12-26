#include "BCM_Init_System.h"

int main(void) {
	
  config_modes_and_clk();
  enable_Normal_Mode();
  disableWatchdog();
  
  
/*  config_PORT_E();
  initPIT();
  
  //procédure définie dans IntcInterrupts.c : 1e paramètre = procédure associée à la routine ISR
  //60 = numéro dans la table des vecteurs d'interruption, 2 = niveau de priorité.
  INTC_InstallINTCInterruptHandler(PIT1_Interrupt,60,2);
  enableIrq(); // Ensure INTC current prority=0 & enable IRQ 
	
  SIU.PGPDO[2].R = 0x00000000;
*/

  /* Loop forever */
  for (;;) {	    
  }
}