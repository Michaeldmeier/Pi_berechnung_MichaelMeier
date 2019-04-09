void rtc_init(void)
{	
	CCP		= CCP_IOREG_gc;								// Schützt I/O Register, Interrupts werden ignoriert
	OSC.CTRL	|= OSC_RC32KEN_bm;						// Internen RTC Oszillator aktivieren
	CLK.RTCCTRL	= CLK_RTCSRC_RCOSC_gc|CLK_RTCEN_bm;		// Internen Oszillator mit 1024Hz für RTC verwenden
	RTC.CTRL	= RTC_PRESCALER_DIV1_gc;				// Vorteiler auf 1
	while(RTC.STATUS & RTC_SYNCBUSY_bm);				// Warten bis Takt und RTC synchronisiert ist
	RTC.PER		= 1024;									//Timertopwert einstellen
	RTC.INTCTRL |= RTC_OVFINTLVL_HI_gc;					//Timeroverflow Interrupt mit Interrupt Priorität Hoch einstellen
	RTC.CNT		= 0;									//Timerregister CNT auf 0 stellen
	RTC.COMP	= 2;									//Timerregister COMP auf 2 stellen
}