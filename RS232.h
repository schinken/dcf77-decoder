#ifndef _H_RS232_
#define _H_RS232_

//#define F_CPU 8000000UL
#define SET_BAUD(baudRate) (unsigned int)(((F_CPU / baudRate) / 16L) - 1);


void RS232_Init(void);
void RS232_PutChar(uint8_t c);
void RS232_WriteString(char *strn);

void RS232_PrintNumber(unsigned int iNumber);

#endif
