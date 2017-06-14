/*Robinson Paniagua
 * Switch Induced Interrupts
 * main.c
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"


volatile uint32_t Value =0, Value2=0;
void PortFIntHandler()
{
	uint32_t ButtonInterrupt,interruptStatus=0;
	ButtonInterrupt=GPIO_INT_PIN_4;
	interruptStatus = GPIOIntStatus(GPIO_PORTF_BASE,GPIO_PIN_4);
	GPIOIntClear(GPIO_PORTF_BASE,interruptStatus);
	//ButtonInterrupt = GPIO_INT_PIN_4;

	if((interruptStatus&ButtonInterrupt) == 0) //Bool Function, if Switch 2 is depressed then execute the Interrupt
	{
		uint32_t Value =0;
		Value = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4);   //Gives a value to Switch1 and Switch 2 into memory
		Value2 = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0);

		if((Value|Value2)==0)  //If both Switches are pressed, Causes an INterrupt, Red LED turns on
		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0x02); //Red Led
		SysCtlDelay(5000000);
	}


}

 int main(void) {

/********************************* GPIO Port F Configuration************************************************************************************************/
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); //Initializes the Clock SysDiv_5, 40MHZ
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);  //Initializes the GPIOPort A
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  //Initializes Port F
	SysCtlDelay(5);
	GPIO_PORTF_LOCK_R = 0x4C4F434B;             //Unlocks GPIO Port F in order to use Pin0 and Pin 4
	GPIO_PORTF_CR_R = 0x1F;                     //Unlocks Pin 0 & 4 in the GPIO Port F
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_4 |GPIO_PIN_0 ); //Sets GPIO Pin 0 & 4 as inputs
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU); //Sets GPIO Pin as Weak Pull Up Resistor, Negative Logic
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); //Sets the LED's as Outputs->GPIO Pins 1,2,3
/**********************************Interrupt Handler******************************************************************************************************/
	GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_FALLING_EDGE); //Trigger Interrupt on Falling Edge
	GPIOIntRegister(GPIO_PORTF_BASE,PortFIntHandler); //Initializes the Port F handler Sequence
	GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_4);   //Initializes SW 2 as the interrupt
	GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_0);
	GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0); //Reads the GPIO Pins 0 & 4, corresponds to Switch 1 and Switch 2
/***********************************************************************************************************************************************************/




	uint32_t Pin4Value = 0, Pin0Value=0;  //Sets Pin 0 & 4 as variables, initialized to 0

	while(1)
	{
		PortFIntHandler();
		Pin4Value = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4);   //Read Pin 0 and Pin 4 value = Switch 1,2
		Pin0Value = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0);
		if((Pin0Value|Pin4Value)==0) //Pipe Operator OR Logic..if both Button are pressed gives 0 then turn on Pin 1
		{
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0x08); //Turn on GPIO Pin 1 Led Red when both buttons pressed, 0x02 gives 1 to bit 2 on the register
			SysCtlDelay(700000);                          //Delay, compensates for mechanical bounce
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0x00); //Turn off LED
		}

     if((!Pin4Value|Pin0Value)==0)   // if Switch 1, pin 0 is pressed then turn on Blue LED -> GPIO_Pin_2
     {
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0x04);        //0x40 -> sets 3rd bit to 1; 3rd bit corresponds to Pin2
			SysCtlDelay(700000);
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0x00);
     }

     if((Pin4Value|!Pin0Value)==0)      //If Switch 2 is pressed then turn on Green Led -> Pin 3
		{
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0x08); //Sets GPIO Pin 3 to 0x08; Pin 3 -> 4th bit 1000
			SysCtlDelay(700000);
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0x00);
		}
	}

 }
