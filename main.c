#include "main.h"

unsigned char second, minute, hour;

// DCF TOGGLE INTERRUPT
ISR(INT0_vect) {
    DCF_Signal((PIND & 0x04));
}


ISR(TIMER0_OVF_vect){    
    DCF_AddCounter(1, (PIND & 0x04));
    TCNT0=131;
}

ISR(TIMER2_OVF_vect) {

   second++;
   if(second%10 == 0) {

    	RS232_WriteString("Current Time: ");

    	RS232_PrintNumber((unsigned int) hour);
    	RS232_WriteString(":");

    	RS232_PrintNumber((unsigned int) minute);
    	RS232_WriteString(":");

    	RS232_PrintNumber((unsigned int) second);

    	RS232_WriteString("\r\n");
   }

   if ( second == 60 ) {

        second = 0;
        minute++;

        if ( minute == 60 ) {

            minute = 0;
            hour++;

            if ( hour == 24 ){
                hour = 0;
            }
         }
   }
}

void UpdateTime(unsigned char ucHour, unsigned char ucMin) {

    second = 0;
    minute = ucMin;
    hour   = ucHour;

    RS232_WriteString("Updating Time...\r\n");
}


int main(void) {

    
    // Timer/Counter 2 initialization
    ASSR=0x08;

    TCNT2 = 0x00;
    TCCR2 = 0x05;

    OCR2=0x00;

    //==================


    DDRA = 0xFF;
    DDRD |= 0x60;
    PORTB = 0x00;

    MCUCR |= 0x01;		// Toggle Interrupt
    GIMSK |= (1<<INT0); // Schalte externer Interrupt 0 an

    // 1ms Timer Interrupt @ 8MHz
    TCCR0=3;
    TCNT0=131; //0xBF;

    OCR0=0x00;

    TIMSK=0x41;

    DCF_Configure();
    DCF_RegisterUpdate(UpdateTime);

    RS232_Init();
    RS232_WriteString("PINs Configures... enabling Interrupts!\r\n");

    sei();

    do{
    	
    } while(1);

    return 0;
}
