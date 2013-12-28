#include "BCM_Config_System.h"
#include "BCM_System_Mgr.h"

int main(void) {
	
  config_modes_and_clk();
  config_PORT_E();
  initPIT();
  init_Light_Manager();
  init_Watchdog_Manager_Mgr();
  enable_Degraded_Mode();
  enableIrq();
  
  // TODO:
  	// Manage all received CAN messages from LCM
  	// Manage all PS Supervisor problems
  	// Allumer les phares avec le capteur
  	// Corriger le bug du STOP0 mode
  
  
  
  

  /* Loop forever */
  for (;;) {	    
  }
}