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
float64_t pii=0;
int zeitlauft=0;
int zeit=0;
uint64_t counter=0;
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
	float64_t floatpii=0;
	float64_t float0=0;
	float64_t float00001=0;
	int8_t hilf1=0;
	int8_t hilf2=0;
	int8_t hilf3=0;
	int8_t hilf4=0;
	for(;;) {
		floatpii=f_sd(3.141595);
		float0=f_sd(0);
		float00001=f_sd(0.000005);
		hilf1=f_compare(f_sub(floatpii, pii),float0);
		hilf2=f_compare(float00001,f_sub(floatpii, pii));
		hilf3=f_compare(f_sub(pii,floatpii),float0);
		hilf4=f_compare(float00001,f_sub(pii,floatpii));
		if((hilf3>0&&hilf4>0)||(hilf1>0&&hilf2>0)){
			zeitlauft=0;
		}		
		char* tempResultString = f_to_string(pii, 16, 16);		//Verwandeln einer Double-Variable in einen String
		sprintf(Pistring, "1: %s", tempResultString);			//Einsetzen des Strings in einen anderen String
		vDisplayClear();										//Löschen des ganzen Displays
		vDisplayWriteStringAtPos(0,0,"Pi Ausgabe");				//Ausgabe auf das Display Text
		vDisplayWriteStringAtPos(1,0,"%s", Pistring);			//Ausgabe auf das Display pi
		if((xEventGroupGetBits(piieventgroup)&4)==4)			//Status EventBit abfragen
		{
			vDisplayWriteStringAtPos(2,0,"Running");			//Status ausgeben
		}
		else
		{
			vDisplayWriteStringAtPos(2,0,"Stopped");			//Status ausgeben
		}
		vDisplayWriteStringAtPos(3,0,"Zeit:%ds", zeit);			//Zeit ausgeben
		vTaskDelay(500 / portTICK_RATE_MS);
	}
}
void vButtonTask(void *pvParameters) {
	(void) pvParameters;
	for(;;) {
		updateButtons();
		if(getButtonPress(BUTTON1)== SHORT_PRESSED)				//Button 1 gedrückt
		{
			zeitlauft=1;										//Zeit starten
			xEventGroupSetBits(piieventgroup, BIT_0);			//Status EventBit setzen
		}
		if(getButtonPress(BUTTON2)== SHORT_PRESSED)				//Button 2 gedrückt
		{
			zeitlauft=0;										//Zeit stoppen
			xEventGroupClearBits(piieventgroup, BIT_0);			//Status EventBit löschen
		}
		if(getButtonPress(BUTTON3)== SHORT_PRESSED)				//Button 3 gedrückt
		{
			xEventGroupSetBits(piieventgroup, BIT_1);			//Reset EventBit setzen
		}
		vTaskDelay(10 / portTICK_RATE_MS);
	}
}
void vCalcPiTask(void *pvParameters) {
	(void) pvParameters;		
	float64_t Zwischenspeicher1=0;
	float64_t Zwischenspeicher2=0;
	float64_t Zwischenspeicher3=0;
	float64_t Zwischenspeicher4=0;
	float64_t Zwischenspeicher5=0;
	float64_t Zwischenspeicher6=0;
	float64_t Zwischenspeicher7=0;
	PORTE.DIRSET = PIN0_bm; /*LED1*/
	PORTE.OUT = 0x01;
	for(;;) 
	{
		if((xEventGroupGetBits(piieventgroup)&1)==1)
		{
			if(counter%2)
			{
				Zwischenspeicher1=f_mult(Zwischenspeicher2,f_sd(2));
				Zwischenspeicher3=f_add(Zwischenspeicher1,f_sd(1));
				Zwischenspeicher4=f_div(f_sd(4.0),Zwischenspeicher3);
				pii=f_sub(pii, Zwischenspeicher4);
				Zwischenspeicher2=f_add(Zwischenspeicher2, f_sd(1));
				counter++;
			}
			else
			{
				Zwischenspeicher5=f_mult(Zwischenspeicher2,f_sd(2));
				Zwischenspeicher6=f_add(Zwischenspeicher5,f_sd(1));
				Zwischenspeicher7=f_div(f_sd(4.0),Zwischenspeicher6);
				pii=f_add(pii, Zwischenspeicher7);
				Zwischenspeicher2=f_add(Zwischenspeicher2, f_sd(1));
				counter++;
			}
			xEventGroupSetBits(piieventgroup, BIT_2);
		}
		else
		{
			xEventGroupClearBits(piieventgroup, BIT_2);
			
		}
		if((xEventGroupGetBits(piieventgroup)&2)==2)
		{
				counter=0;
				Zwischenspeicher1=0;
				Zwischenspeicher2=0;
				Zwischenspeicher3=0;
				Zwischenspeicher4=0;
				Zwischenspeicher5=0;
				Zwischenspeicher6=0;
				Zwischenspeicher7=0;
				pii=0;
				zeit=0;
				xEventGroupClearBits(piieventgroup, BIT_1);
		}
	}
}
