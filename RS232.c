#include "main.h"


void RS232_Init() {

    unsigned int baudRate = SET_BAUD(38400);
    UBRRL = baudRate;
    UBRRH = baudRate >> 8;

    UCSRB |= (1 << TXEN); 
    //I don't think this makes much of a difference
    UCSRC = ((1 << URSEL) | (3 << UCSZ0));
}

void RS232_PutChar(uint8_t c) {
    UDR = c;
    while(bit_is_clear(UCSRA, TXC));
    //clear the bit by writing a 1
    UCSRA = 1 << TXC;
}

void RS232_WriteString(char *strn) {

    while(*strn) {
        RS232_PutChar(*(strn++));
    }
}

void RS232_PrintNumber(unsigned int iNumber) {

    char cBuffer[10];	
    unsigned char cLength = sprintf(cBuffer, "%d", iNumber);

    unsigned char cCount;

    for(cCount=0;cCount < cLength; cCount++) {
    	RS232_PutChar(cBuffer[cCount]);
    }

}

