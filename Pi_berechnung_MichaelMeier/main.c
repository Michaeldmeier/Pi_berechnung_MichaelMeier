/*
 * Pi_berechnung_MichaelMeier.c
 *
 * Created: 20.03.2018 18:32:07
 * Author : Michael Meier
 */ 

//#include <avr/io.h>
#include <stdio.h>

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
char Pistring[20];


extern void vApplicationIdleHook( void );
void vDisplayTask(void *pvParameters);
//void vButtonTask(void *pvParameters);
void vCalcPiTask(void *pvParameters);

TaskHandle_t PiTask;

void vApplicationIdleHook( void )
{	
	
}

int main(void)
{
    resetReason_t reason = getResetReason();

	vInitClock();
	vInitDisplay();
	
	xTaskCreate( vDisplayTask, (const char *) "Display", configMINIMAL_STACK_SIZE+300, NULL, 2, &PiTask);
//	xTaskCreate( vButtonTask, (const char *) "Button", configMINIMAL_STACK_SIZE+10, NULL, 1, &PiTask);
	xTaskCreate( vCalcPiTask, (const char *) "CalcPi", configMINIMAL_STACK_SIZE+300, NULL, 1, &PiTask);

	vDisplayClear();
	vTaskStartScheduler();
	return 0;
}

void vDisplayTask(void *pvParameters) {
	(void) pvParameters;
	PORTE.DIRSET = PIN0_bm; /*LED1*/
	PORTE.OUT = 0x01;
	for(;;) {
		PORTE.OUTTGL = 0x01;
		
		vDisplayClear();										//Löschen des ganzen Displays
		vDisplayWriteStringAtPos(0,0,"Pi Ausgabe");				//Ausgabe auf das Display
		vDisplayWriteStringAtPos(1,0,"%s", Pistring);
		vTaskDelay(500 / portTICK_RATE_MS);
	}
}
/*void vButtonTask(void *pvParameters) {
	(void) pvParameters;
	PORTF.DIRSET = PIN0_bm; //LED1
	PORTF.OUT = 0x01;
	for(;;) {
		PORTF.OUTTGL = 0x01;
		vTaskDelay(100 / portTICK_RATE_MS);
	}
}*/
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
		float64_t pii=0;
		for(;;) 
		{
			if(m%2)
			{
				n=f_mult(o,f_sd(2));
				p=f_add(n,f_sd(1));
				q=f_div(f_sd(4),p);
				pii=f_sub(pii, q);
				o=f_add(o, f_sd(1));
				m++;
			}
			else
			{
				//h=(-4/(2*m+1));
				//pivar=f_add(pivar,(float64_t)h);
				r=f_mult(o,f_sd(2));
				s=f_add(r,f_sd(1));
				t=f_div(f_sd(4),s);
				pii=f_add(pii, t);
				o=f_add(o, f_sd(1));
				m++;
			}
			//pivar=f_mult(pi4var,f_sd(4));
		char* tempResultString = f_to_string(pii, 16, 16);		//Verwandeln einer Double-Variable in einen String
		sprintf(Pistring, "1: %s", tempResultString);				//Einsetzen des Strings in einen anderen String
	
	}
}
