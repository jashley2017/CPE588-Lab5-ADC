/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           APPLICATION CODE
*
*                                      Texas Instruments TM4C129x
*                                                on the
*                                             DK-TM4C129X
*                                           Development Kit
*       		Modified by Dr. Samir A. Rawashdeh, for the TM4C123GH6PM microcontroller on the 
*						TM4C123G Tiva C Series Launchpad (TM4C123GXL), November 2014.
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : FF
*********************************************************************************************************
* Note(s)       : None.
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  "app_cfg.h"
#include  <cpu_core.h>
#include  <os.h>

#include  "..\bsp\bsp.h"
#include  "..\bsp\bsp_led.h"
#include  "..\bsp\bsp_sys.h"

// SAR Addition
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "utils/uartstdio.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK       AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK       Task1Stk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK       Task2Stk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK       Task3Stk[APP_CFG_TASK_START_STK_SIZE];

/*
*********************************************************************************************************
*                                            LOCAL MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskCreate         (void);
static  void  AppTaskStart          (void       *p_arg);
static  void  Task1          (void       *p_arg);
static  void  Task2          (void       *p_arg);
static  void  Task3          (void       *p_arg);

/*$PAGE*/
/*
*********************************************************************************************************
*                                               main()
*
* Description : Entry point for C code.
*
* Arguments   : none.
*
* Returns     : none.
*
* Note(s)     : (1) It is assumed that your code will call main() once you have performed all necessary
*                   initialization.
*********************************************************************************************************
*/
OS_EVENT *UARTMutex;


int  main (void)
{
#if (OS_TASK_NAME_EN > 0)
    CPU_INT08U  err;
#endif

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR     cpu_err;
#endif

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_NameSet((CPU_CHAR *)"TM4C129XNCZAD",
                (CPU_ERR  *)&cpu_err);
#endif

    CPU_IntDis();                                               /* Disable all interrupts.                              */

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"          */

    OSTaskCreateExt((void (*)(void *)) AppTaskStart,           /* Create the start task                                */
                    (void           *) 0,
                    (OS_STK         *)&AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_START_PRIO,
                    (INT16U          ) APP_CFG_TASK_START_PRIO,
                    (OS_STK         *)&AppTaskStartStk[0],
                    (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_CFG_TASK_START_PRIO, "Start", &err);
#endif
										
		UARTMutex = OSMutexCreate(9,&err);

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)   */

    while (1) {
        ;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           App_TaskStart()
*
* Description : Startup task example code.
*
* Arguments   : p_arg       Argument passed by 'OSTaskCreate()'.
*
* Returns     : none.
*
* Created by  : main().
*
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
		CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    (void)p_arg;                                                /* See Note #1                                              */


   (void)&p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */

    cpu_clk_freq = BSP_SysClkFreqGet();                         /* Determine SysTick reference freq.                    */
    cnts         = cpu_clk_freq                                 /* Determine nbr SysTick increments                     */
                 / (CPU_INT32U)OS_TICKS_PER_SEC;

    OS_CPU_SysTickInit(cnts);
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                                   */
#endif

    Mem_Init();

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif


		BSP_LED_Toggle(0);
		OSTimeDlyHMSM(0, 0, 0, 200);
		BSP_LED_Toggle(0);
		BSP_LED_Toggle(1);
		OSTimeDlyHMSM(0, 0, 0, 200);
		BSP_LED_Toggle(1);
		BSP_LED_Toggle(2);
		OSTimeDlyHMSM(0, 0, 0, 200);    
		BSP_LED_Toggle(2);

		OSTimeDlyHMSM(0, 0, 1, 0);   

		AppTaskCreate();                                            /* Creates all the necessary application tasks.         */

    while (DEF_ON) {

        OSTimeDlyHMSM(0, 0, 0, 200);			

    }
}


/*
*********************************************************************************************************
*                                         AppTaskCreate()
*
* Description :  Create the application tasks.
*
* Argument(s) :  none.
*
* Return(s)   :  none.
*
* Caller(s)   :  AppTaskStart()
*
* Note(s)     :  none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
OSTaskCreate((void (*)(void *)) Task1,           /* Create the first task                                */
                    (void           *) 0,							// argument
                    (OS_STK         *)&Task1Stk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) 7 );  						// Task Priority
                

OSTaskCreate((void (*)(void *)) Task2,           /* Create the second task                                */
                    (void           *) 0,							// argument
                    (OS_STK         *)&Task2Stk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) 6 );  						// Task Priority
									
OSTaskCreate((void (*)(void *)) Task3,           // Create the third task                                
                    (void           *) 0,							// argument
                    (OS_STK         *)&Task3Stk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) 5 );  						// Task Priority     										
}

static  void  Task1 (void *p_arg)
{
	  uint8_t err;
		uint32_t ui32Value;
	 // float voltage;
   (void)p_arg;
		
	//
	// Enable the ADC0 module.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	//
	// Wait for the ADC0 module to be ready.
	//
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
	{
	}
	//
	// Enable the first sample sequencer to capture the value of channel 0 when
	// the processor trigger occurs.
	//
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0,
	ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
	ADCSequenceEnable(ADC0_BASE, 0);
	//
	// Trigger the sample sequence.
	//
	
	//
	// Wait until the sample sequence has completed.
	//
	while(1) { 
		ADCProcessorTrigger(ADC0_BASE, 0);
		
		while(!ADCIntStatus(ADC0_BASE, 0, false)){}
		//
		// Read the value from the ADC.
		//
		ADCSequenceDataGet(ADC0_BASE, 0, &ui32Value); // uint32_t
		OSMutexPend(UARTMutex, 0, &err); // acquire lock for the uart
		//voltage = ui32Value/4095;
		UARTprintf("%u\n", ui32Value);
		OSMutexPost(UARTMutex); // release lock for the uart
			OSTimeDlyHMSM(0, 0, 0, 5);
	}
	/*
    while (1) {              
        BSP_LED_Toggle(1);
			  OSMutexPend(UARTMutex, 0, &err); // acquire lock for the uart
				UARTprintf("T1234567891011121314\n");	// Probably needs to be protected by semaphore
        OSMutexPost(UARTMutex); // release lock for the uart
			  BSP_LED_Toggle(1);
				OSTimeDlyHMSM(0, 0, 0, 2);
				//OSTimeDly(2);

			}
*/
}

static  void  Task2 (void *p_arg)
{
	 uint8_t err;
   (void)p_arg;
    while (1) {              
        BSP_LED_Toggle(2);
				OSMutexPend(UARTMutex, 0, &err); // acquire lock for the uart
  			UARTprintf("TABCDEFGHIJKLMNOPQRSTUV\n");  // Probably needs to be protected by semaphore
				OSMutexPost(UARTMutex); // release lock for the uart
        BSP_LED_Toggle(2);
			  OSTimeDlyHMSM(0, 0, 0, 2); // delay 1ms, without the mutex's this will make the UART's clash at 80MHz
			  //OSTimeDly(2);
			}
}
static  void  Task3 (void *p_arg)
{
	 uint8_t err;
   (void)p_arg;
    while (1) {              
        BSP_LED_Toggle(0);
				OSMutexPend(UARTMutex, 0, &err); // acquire lock for the uart
  			UARTprintf("TTASK3TASK3TASK3TASK3\n");  // Probably needs to be protected by semaphore
				OSMutexPost(UARTMutex); // release lock for the uart
        BSP_LED_Toggle(0);
			  OSTimeDlyHMSM(0, 0, 0, 2); // delay 1ms, without the mutex's this will make the UART's clash at 80MHz
			  // OSTimeDly(2);
			}
}