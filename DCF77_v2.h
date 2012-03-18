#ifndef _H_DCF77_
#define _H_DCF77_

////////////////////////////////////////////////////////////////////////
//
// DCF77 DECODER HEADER FILE
//
// AUTHOR:     CHRISTOPHER SCHIRNER
// DATE:    01.02.2007
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// RAW TIME STRUCTURE
////////////////////////////////////////////////////////////////////////

struct tRawTime
{
    unsigned char ucMinute;
    unsigned char ucHour;
    unsigned char ucDay;
    unsigned char ucMonth;
    unsigned char ucYear;
    unsigned char ucWeekDay;
};

typedef void (*RegFunc) (unsigned char, unsigned char);

////////////////////////////////////////////////////////////////////////
// PROTOTYPES
////////////////////////////////////////////////////////////////////////

void DCF_Configure(void);
void DCF_Signal(unsigned char cLevel);
void DCF_ResetCounter(void);
void DCF_AddCounter(unsigned char cMs, unsigned char cSignal);
void DCF_AnalyseBit(void);
bool DCF_CheckParityPartial(unsigned char iBegin, unsigned char iEnd);
bool DCF_CheckParity(void);
void DCF_SignalControl(void);
void DCF_ConvertRaw(void);
struct tRawTime DCF_GetTime(void);
char DCF_BcdToDec(unsigned char cBegin, unsigned char cEnd);
bool DCF_GetErrorState(void);
void DCF_ArrayPointer(void);
void DCF_CallUpdate(void);
void DCF_RegisterUpdate(RegFunc pFunction);


#endif
