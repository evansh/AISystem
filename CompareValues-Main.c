//----------------------------------------------------------------------------------
//	FILE:	CompareValues-Main.c
//
//  Description:  This example configures ePWM1A,Comparator1 and internal DAC. Depending 
//	on comparator output the LED turns on and off.In this example, the user can compare 
//	either PWM1A-DAC output or external supply voltage with internal DAC.	   
//	
//    Initially make internal DAC output ('dac_value') lower than V on COMP1A. 
//    
//    During the test observe the on board LED. 
//    Increase the voltage value on inverting side of comparator through 
//	  "dac_value" from watch window to trigger DCAEVT2. The other option is decreasing
//	  the external supply voltage or PWM1A duty cycle ('duty_cycle_A') to pull the
//	  comparator output low. 

//    DCAEVT2 is defined as true when COMP1OUT is low.
//    DCAEVT2 is a Cycle-By-Cycle trip source for ePWM1		
//
//  Target:  		TMS320F2806x or TMS320F2803x families (F28069)
//
//----------------------------------------------------------------------------------
//  $TI Release:$ 	V1.0
//  $Release Date:$ 11 Jan 2010 - VSC
//----------------------------------------------------------------------------------
//
// PLEASE READ - Useful notes about this Project

// Although this project is made up of several files, the most important ones are:
//	    - "CompareValues.c",	this file
//		- Application Initialization, Peripheral config
//		- Application management
//		- Slower background code loops and Task scheduling
//	 "CompareValues-DevInit_F28xxx.c"
//		- Device Initialization, e.g. Clock, PLL, WD, GPIO mapping
//		- Peripheral clock enables
// The other files are generally used for support and defining the registers as C
// structs. In general these files will not need to be changed.
//   "F2806x_RAM_CompareValues.CMD" or "F2806x_FLASH_CompareValues.CMD"
//		- Allocates the program and data spaces into the device's memory map.
//   "F2806x_Headers_nonBIOS.cmd" and "F2806x_GlobalVariableDefs.c"
//		- Allocate the register structs into data memory.  These register structs are
//		  defined in the peripheral header includes (F2806x_Adc.h, ...) 
//
//----------------------------------------------------------------------------------

#include "PeripheralHeaderIncludes.h"
#include "F2806x_EPwm_defines.h" 	    // useful defines for initialization
#include "stdio.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// FUNCTION PROTOTYPES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void DeviceInit(void);
void InitFlash();
void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr);
void test(void);
void sleepFunc(int delay);
void restart(void);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// VARIABLE DECLARATIONS - GENERAL
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Used for running BackGround in flash and the ISR in RAM
extern Uint16 RamfuncsLoadStart, RamfuncsLoadEnd, RamfuncsRunStart;

Uint16 dac_value= 310;			// Set as ~1V initially
								// Internal DAC range is 0-1024 resulting 0-3.3V
Uint16 duty_cycle_A=20;			// Set duty 50% initially, ~1.5 V
							
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// MAIN CODE - starts here
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void main(void)
{
//=================================
//	INITIALISATION - General
//=================================

	DeviceInit();	// Device Life support & GPIO mux settings

// Only used if running from FLASH
// Note that the variable FLASH is defined by the compiler (-d FLASH)
#ifdef FLASH		
// Copy time critical code and Flash setup code to RAM
// The  RamfuncsLoadStart, RamfuncsLoadEnd, and RamfuncsRunStart
// symbols are created by the linker. Refer to the linker files. 
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

// Call Flash Initialization to setup flash waitstates
// This function must reside in RAM
	InitFlash();	// Call the flash wrapper init function
#endif //(FLASH)

   EALLOW;

   AdcRegs.ADCCTL1.bit.ADCBGPWD = 1;        //Comparator shares the internal BG reference of the ADC, must be powered even if ADC is unused
   Comp1Regs.COMPCTL.bit.COMPDACEN = 1;     //Power up Comparator 1 locally                                 
   Comp1Regs.COMPCTL.bit.CMPINV = 1;
   // Comp1Regs.COMPCTL.bit.COMPSOURCE = 1;    //Connect the inverting input to pin COMP1B
											//Uncomment previous line to use COMP1B
											//instead of internal DAC 
   Comp1Regs.COMPCTL.bit.COMPSOURCE = 0;    //Connect the inverting input to the internal DAC
   Comp1Regs.DACVAL.bit.DACVAL =dac_value;	//Set DAC output to midpoint


   Comp2Regs.COMPCTL.bit.COMPDACEN = 1;     //Power up Comparator 1 locally
   Comp2Regs.COMPCTL.bit.COMPSOURCE = 1;    //Connect the inverting input to pin COMP1B
										//Uncomment previous line to use COMP1B
										//instead of internal DAC
   //Comp2Regs.COMPCTL.bit.COMPSOURCE = 0;    //Connect the inverting input to the internal DAC
   //Comp2Regs.DACVAL.bit.DACVAL =dac_value;	//Set DAC output to midpoint

   EDIS;

//===============================================================
// PWM-DAC & TZ Configuration 
//===============================================================

#define period 40							  // 2MHz when PLL is set to 0x10 (80MHz) 
 
  // Time-base registers
   	EPwm1Regs.TBPRD = period;       		   // Set timer period, PWM frequency = 1 / period
   	EPwm1Regs.TBPHS.all = 0;				   // Time-Base Phase Register
   	EPwm1Regs.TBCTR = 0;					   // Time-Base Counter Register	
    EPwm1Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;  // Set Immediate load
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count-up mode: used for asymmetric PWM
	EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;	   // Disable phase loading
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
	EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

   	// Setup shadow register load on ZERO

   	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;	// load on CTR=Zero

   	// Set Compare values

   	EPwm1Regs.CMPA.half.CMPA = 20;    		// Set duty 50% initially, ~1.5 V
   	
   	EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;      // Set PWM2A on Zero
   	EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;    // Clear PWM2A on event A, up count

		
	EALLOW;
//================================================
// Configure digital compare event (DCAEVT2)
//================================================
    EPwm1Regs.DCTRIPSEL.bit.DCAHCOMPSEL = DC_COMP1OUT;   // DCAH = Comparator 1 output
    EPwm1Regs.DCTRIPSEL.bit.DCALCOMPSEL = DC_TZ2;        // DCAL = TZ2
    EPwm1Regs.TZDCSEL.bit.DCAEVT2 = TZ_DCAH_LOW; 		 // DCAEVT2 =  DCAH low(will become active as Comparator output goes low)  
    EPwm1Regs.DCACTL.bit.EVT1SRCSEL = DC_EVT2;           // DCAEVT2 = DCAEVT2 (not filtered)
    EPwm1Regs.DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;  // Take async path
   
    // Enable DCAEVT2 and DCBEVT2 are cycle-by-cycle trip sources
    // Note:  DCxEVT1 events can be defined as one-shot.  
    //        DCxEVT2 events can be defined as cycle-by-cycle.

    EPwm1Regs.TZSEL.bit.DCAEVT2 = 1;
    EPwm1Regs.TZSEL.bit.DCBEVT2 = 1;
	
    EPwm1Regs.TZCTL.bit.TZA = TZ_NO_CHANGE;           // Disable TZ actions
    EPwm1Regs.TZCTL.bit.DCAEVT2 = TZ_NO_CHANGE;		  // Disable DCAEVT2 actions
	EPwm1Regs.TZCLR.all = 0xFFFF;
	EDIS;

//=================================
//	INTERRUPT INITIALISATION (not needed in this example)
//  (best to run this section after other initialisation)
//=================================

// Enable Peripheral, global Ints and higher priority real-time debug events:
//	IER |= M_INT3; 
//	EINT;   // Enable Global interrupt INTM
//	ERTM;   // Enable Global realtime interrupt DBGM

	test();
//=================================
//	Forever LOOP
//=================================
	for(;;)  //infinite loop
	{
		/*
		 * Clock
		 * Once comparator flips the channel off
		 * you must redo the clock to start back up
		 *
		*GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0;   // Enable pullup on GPIO1
		GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;  // GPIO1 = GPIO1
		GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;   // GPIO1 = output
		GpioDataRegs.GPASET.bit.GPIO0 = 1;   // Load output latch
		 *
		 */

		/*
		 * Enable Pin
		 * GpioDataRegs.GPASET.bit.GPIO2 = 1;   // Load output latch
		 */

		if(EPwm1Regs.TZFLG.bit.DCAEVT2==0)		// Turn on LED when the comparator output goes low.
												// LED will be turned on until the comparator goes high 
		   GpioDataRegs.GPBSET.bit.GPIO34 = 1;  // as DCAEVT2 is cycle by cycle event controller.
			
		else
		   GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;

		GpioDataRegs.GPASET.bit.GPIO12 = 1;
		sleepFunc(100000);
		GpioDataRegs.GPACLEAR.bit.GPIO12 = 1;
		if (GpioDataRegs.GPADAT.bit.GPIO1 == 0) {
			printf("Press any key to continue: ");
			getchar();
		 }
	    EALLOW;
   		EPwm1Regs.TZCLR.bit.DCAEVT2 =0xFFFF;		// Clear the DCAEVT2 flag. The flag status will be   
		restart();
		EDIS;										// updated in each cycle.

		Comp1Regs.DACVAL.bit.DACVAL =dac_value;	    // Adjust (internal) DAC value
		EPwm1Regs.CMPA.half.CMPA = duty_cycle_A;	// Adjust PWM1A-DAC output
   	}												
  
  //=================================
 //	Forever LOOP
  //=================================
  // Just sit and loop forever:
 	
}

void sleepFunc(int delay){
     int count;
     for(count = 0; count < delay; count++);
}

void restart(void)
{
	EALLOW;
	/*
		 * Turn Channel On
		 * Use in this order
		 */
		GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;   // Enable pullup on GPIO1
		GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;  // GPIO1 = GPIO1
		GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;   // GPIO1 = output
		GpioDataRegs.GPASET.bit.GPIO3 = 1;   // Load output latch

		GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;   // Enable pullup on GPIO2
		GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;  // GPIO2 = GPIO2
		GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;   // GPIO2 = output
		GpioDataRegs.GPASET.bit.GPIO2 = 1;   // Load output latch

		GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;   // Enable pullup on GPIO2
		GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;  // GPIO2 = GPIO2
		GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;   // GPIO2 = output
		GpioDataRegs.GPASET.bit.GPIO1 = 1;   // Load output latch


		GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0;   // Enable pullup on GPIO1
		GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;  // GPIO1 = GPIO1
		GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;   // GPIO1 = output
		// We need to clear then set the bit so that we know the clock changes
		GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;   // Load output latch
		GpioDataRegs.GPASET.bit.GPIO0 = 1;   // Load output latch


		GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 3;  // GPIO2 = GPIO2
		EDIS;	// Disable register access

}


void test(void)
{
	//InitGpio();
	EALLOW;

	GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;   // Enable pullup on GPIO1
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 3;  // GPIO1 = GPIO1
	GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;   // GPIO1 = output
	//GpioDataRegs.GPASET.bit.GPIO3 = 1;   // Load output latch
	//GpioDataRegs.GPADAT.bit.GPIO3

	// Set to output for test
//	GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;   // Enable pullup on GPIO1
//	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;  // GPIO1 = GPIO1
//	GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;
//
	GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;   // Enable pullup on GPIO2
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;  // GPIO2 = GPIO2
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;   // GPIO2 = output
	//GpioDataRegs.GPASET.bit.GPIO2 = 1;   // Load output latch

	// Make GPIO32 an output
//	GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;   // Enable pullup on GPIO34
//	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;  // GPIO32 = GPIO32
//	GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;   // GPIO32 = output
//	GpioDataRegs.GPBSET.bit.GPIO32 = 1;   // Load output latch
//
	GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;   // Enable pullup on GPIO2
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;  // GPIO2 = GPIO2
	GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;   // GPIO2 = output
	//GpioDataRegs.GPASET.bit.GPIO1 = 1;   // Load output latch
//
//	GpioDataRegs.GPADAT.bit.GPIO1
	GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0;   // Enable pullup on GPIO1
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;  // GPIO1 = GPIO1
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;   // GPIO1 = output
	GpioDataRegs.GPASET.bit.GPIO0 = 1;   // Load output latch

//	GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;
//	GpioDataRegs.GPASET.bit.GPIO1 = 0;   // Load output latch
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 3;  // GPIO2 = GPIO2


	EDIS;
}
