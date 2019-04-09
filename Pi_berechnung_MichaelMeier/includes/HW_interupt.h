void HW_interupt(void)
{
	#define sbi(ADDRESS,BIT) 	ADDRESS |= (1<<BIT)						// set Bit
    #define cbi(ADDRESS,BIT) 	ADDRESS &= ~(1<<BIT)					// clear Bit
	cli();																// alle Interupts ausschalten
	cbi(PORTF.DIR,5);													// Pin 6 löschen
	sbi(PORTF.INT0MASK,5);												// in der Iterruptmaske 0 Pin 6 freigeben
	PORTF.PIN5CTRL	= PORT_OPC_WIREDORPULL_gc | PORT_ISC_FALLING_gc;	// Pin 6 PullUp aktivieren, fallende Flanke aktivieren
	PORTF.INTCTRL	= PORT_INT0LVL_HI_gc;								// Interrupt HiLevel für PORTF und INT0MASK freigeben
	PMIC.CTRL|=PMIC_HILVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_LOLVLEN_bm;		// Interupt aktivieren
	sei();																// alle Interupts einschalten
}