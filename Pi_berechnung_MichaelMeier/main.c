/*
 * Pi_berechnung_MichaelMeier.c
 *
 * Created: 20.03.2018 18:32:07
 * Author : Michael Meier
 */ 

//#include <avr/io.h>
#include <stdio.h>
#include <avr/io.h>				//Headerfile für Ein- und Ausgänge
#include <avr/interrupt.h>		//Headerfile für den Interupt
#include "rtc.h"				//Headerfile für den Realtime Counter welcher ein Interupt auslöst
//#include "HW_interupt.h"		//Headerfile für den Hardware Interupt
#include "avr_compiler.h"
#include "pmic_driver.h"
#include "TC_driver.h"
#include "clksys_driver.h"
#include "sleepConfig.h"
#include "port_driver.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "stack_macros.h"

#include "mem_check.h"

#include "init.h"
#include "utils.h"
#include "errorHandler.h"
#include "NHD0420Driver.h"
#include "avr_f64.h"	
#include "buttonhandler.h"
char Pistring[20];
//long double pii=0;
float64_t pii=0;
int zeitlauft=0;
int zeit=0;
#define BIT_0	( 1 << 0 )
#define BIT_1	( 1 << 1 )
#define BIT_2	( 1 << 2 )


extern void vApplicationIdleHook( void );
void vDisplayTask(void *pvParameters);
void vButtonTask(void *pvParameters);
void vCalcPiTask(void *pvParameters);

TaskHandle_t PiTask;
EventGroupHandle_t piieventgroup;
ISR(RTC_OVF_vect)				//Interupt welcher vom Realtime Counter ausgelöst wird
{
	if(zeitlauft)
	{
		zeit++;					//Zähler um 1 erhöhen
	}
}
void vApplicationIdleHook( void )
{	

}

int main(void)
{
	rtc_init();					//Realtime Counter initialisieren
//	HW_interupt();				//Hardware Interupt initialisieren
    resetReason_t reason = getResetReason();
	piieventgroup = xEventGroupCreate();
	vInitClock();
	vInitDisplay();
	initButtons();
	xTaskCreate( vDisplayTask, (const char *) "Display", configMINIMAL_STACK_SIZE+300, NULL, 2, &PiTask);
	xTaskCreate( vButtonTask, (const char *) "Button", configMINIMAL_STACK_SIZE+300, NULL, 2, &PiTask);
	xTaskCreate( vCalcPiTask, (const char *) "CalcPi", configMINIMAL_STACK_SIZE+300, NULL, 1, &PiTask);

	vDisplayClear();
	vTaskStartScheduler();
	return 0;
}

void vDisplayTask(void *pvParameters) {
	(void) pvParameters;
	float64_t piiout;
	for(;;) {
		//piiout=f_sd(pii);
		//char* tempResultString = f_to_string(piiout, 16, 16);		//Verwandeln einer Double-Variable in einen String
		char* tempResultString = f_to_string(pii, 16, 16);		//Verwandeln einer Double-Variable in einen String
		sprintf(Pistring, "1: %s", tempResultString);			//Einsetzen des Strings in einen anderen String
		vDisplayClear();										//Löschen des ganzen Displays
		vDisplayWriteStringAtPos(0,0,"Pi Ausgabe");				//Ausgabe auf das Display
		vDisplayWriteStringAtPos(1,0,"%s", Pistring);
		if((xEventGroupGetBits(piieventgroup)&4)==4)
		{
			vDisplayWriteStringAtPos(2,0,"Running");
		}
		else
		{
			vDisplayWriteStringAtPos(2,0,"Stopped");
		}
		vDisplayWriteStringAtPos(3,0,"Zeit:%d s", zeit);
		vTaskDelay(500 / portTICK_RATE_MS);
	}
}
void vButtonTask(void *pvParameters) {
	(void) pvParameters;
	for(;;) {
		updateButtons();
		if(getButtonPress(BUTTON1)== SHORT_PRESSED)
		{
			xEventGroupSetBits(piieventgroup, BIT_0);
		}
		if(getButtonPress(BUTTON2)== SHORT_PRESSED)
		{
			xEventGroupClearBits(piieventgroup, BIT_0);
		}
		if(getButtonPress(BUTTON3)== SHORT_PRESSED)
		{
			xEventGroupSetBits(piieventgroup, BIT_1);
		}
		vTaskDelay(10 / portTICK_RATE_MS);
	}
}
void vCalcPiTask(void *pvParameters) {
	(void) pvParameters;		
	uint64_t m=0;
	float64_t n=0;
	float64_t o=0;
	float64_t p=0;
	float64_t q=0;
	float64_t r=0;
	float64_t s=0;
	float64_t t=0;
	PORTE.DIRSET = PIN0_bm; /*LED1*/
	PORTE.OUT = 0x01;
	for(;;) 
	{
		if((xEventGroupGetBits(piieventgroup)&1)==1)
		{
			if(m%2)
			{
				//pii=pii-(4.0/(2.0*m+1.0));
				n=f_mult(o,f_sd(2));
				p=f_add(n,f_sd(1));
				q=f_div(f_sd(4.0),p);
				pii=f_sub(pii, q);
				o=f_add(o, f_sd(1));
				m++;
			}
			else
			{
				//pii=pii+(4.0/(2.0*m+1.0));
				r=f_mult(o,f_sd(2));
				s=f_add(r,f_sd(1));
				t=f_div(f_sd(4.0),s);
				pii=f_add(pii, t);
				o=f_add(o, f_sd(1));
				m++;
			}
			zeitlauft=1;
			xEventGroupSetBits(piieventgroup, BIT_2);
		}
		else
		{
			xEventGroupClearBits(piieventgroup, BIT_2);
			zeitlauft=0;
		}
		if((xEventGroupGetBits(piieventgroup)&2)==2)
		{
				m=0;
				n=0;
				o=0;
				p=0;
				q=0;
				r=0;
				s=0;
				t=0;
				pii=0;
				zeit=0;
				xEventGroupClearBits(piieventgroup, BIT_1);
		}
	}
}
