/******************************************************************************
 *
 * www.payitforward.edu.vn
 *
 *****************************************************************************/

/*
 * main.c
 *      Author: PIF
 *      Editor: minht57
 */

/******************************************************************************
 * IMPORT
 *****************************************************************************/
#include "CC1101/cc1101_define.h"
#include "CC1101/CC1101.h"
/******************************************************************************
 * DEFINE
 *****************************************************************************/
//#define TX

/******************************************************************************
 * GLOBAL VARIABLE
 *****************************************************************************/
uint8_t ui8_Buf[20];
char    c_len = 11;
int8_t  i8_Flag = 0;

/******************************************************************************
 * MAIN
 *****************************************************************************/
void main(void) {
    // Configure clock 80 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    //PortF Enable
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    //Config GPIO that connect to LED - PIN 1,2,3
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    //Turn off all LEDs
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);

    cc1101_init();

	while(1)
	{
#ifdef TX
	    if(i8_Flag)
	    {
	        i8_Flag = 0;
	        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
	    }
	    else
	    {
	        i8_Flag = 1;
	        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
	    }
	    cc1101_forceSend("PIF-RYA2017",11);
	    ROM_SysCtlDelay(ROM_SysCtlClockGet()/10);
#else
        if(i8_Flag)
        {
            i8_Flag = 0;
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
        }
        else
        {
            i8_Flag = 1;
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
        }
       if(cc1101_receivepacket(ui8_Buf, &c_len))
       {
           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
       }
       else
       {
           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
       }
        ROM_SysCtlDelay(ROM_SysCtlClockGet()/10);
#endif
	}
}

/******************************************************************************
 * END OF main.c
 *****************************************************************************/
