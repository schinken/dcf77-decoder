////////////////////////////////////////////////////////////////////////
//
// DCF77 DECODER SOURCE FILE
//
// AUTHOR:  CHRISTOPHER SCHIRNER
// DATE:    01.02.2007
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////

#include "main.h"

////////////////////////////////////////////////////////////////////////
// STATIC CONST (defines)
////////////////////////////////////////////////////////////////////////

static const unsigned char      K_LOGICAL_HIGH      =    200;
static const unsigned char      K_LOGICAL_LOW       =    100;
static const int                K_SIGNAL_PAUSE      =    1300;
static const unsigned char      K_HIGH              =    1;
static const unsigned char      K_LOW               =    0;
static const unsigned char      K_DCF77_VALUES      =    59;    

////////////////////////////////////////////////////////////////////////
// DCF77 STRUCTURE
////////////////////////////////////////////////////////////////////////

char cDCF77[59] = {
    0,                              // 00       Immer 0
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,    // 01-14    dont care :)
    0,                              // 15       0 = normal 1= Reserveantenne
    0,                              // 16       Ankündigung Wechsel Sommer/Winterzeit
    0,0,                            // 17-18    Offset UTC
    0,                              // 19       Ankündigung einer Schaltsekunde
    0,                              // 20       Startbit für Zeitsequenz
    0,0,0,0,0,0,0,                  // 21-27    Minute BCD (21 LSB)
    0,                              // 28       Parity Minute (21-27)
    0,0,0,0,0,0,                    // 29-34    Hours BCD (29 LSB)
    0,                              // 35       Parity Hours (29-34)
    0,0,0,0,0,0,                    // 36-41    Monatstag BCD
    0,0,0,                          // 42-44    Wochentag (1-Montag 7=Sonntag)
    0,0,0,0,0,                      // 45-49    Monat BCD
    0,0,0,0,0,0,0,0,                // 50-57    Jahr BCD (06/07 etc.)
    0                               // 58       Parity für 36-57
};


////////////////////////////////////////////////////////////////////////
// CONST
////////////////////////////////////////////////////////////////////////

const char cBCD[8] = {1,2,4,8,10,20,40,80};

////////////////////////////////////////////////////////////////////////
// VARIABLES
////////////////////////////////////////////////////////////////////////

unsigned int nSignalLength;

bool bSignalError;
unsigned char cArrayPointer;

struct tRawTime tTime;

RegFunc pCallBack;

////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////


void DCF_Configure(void) {

    nSignalLength   = 0;
    cArrayPointer   = 0;
    bSignalError    = true;
    pCallBack       = NULL;
}

////////////////////////////////////////////////////////////////////////

void DCF_RegisterUpdate( RegFunc pFunction ) {
    pCallBack = pFunction;
}

////////////////////////////////////////////////////////////////////////

void DCF_CallUpdate( void ) {
    
    if( pCallBack == NULL ) {
        return;
    }

    (*pCallBack)( tTime.ucHour, tTime.ucMinute );
}

////////////////////////////////////////////////////////////////////////

void DCF_Signal( unsigned char signalLevel ) {

    if( signalLevel != K_LOW ) {

        if( !bSignalError && cArrayPointer == 0 ) {
            DCF_CallUpdate();
        }

    } else {
        DCF_AnalyseBit();
    }

    DCF_ResetCounter();

}

////////////////////////////////////////////////////////////////////////

void DCF_ResetCounter(void) {
    nSignalLength = 0;
}

////////////////////////////////////////////////////////////////////////

void DCF_AddCounter( unsigned char cMs, unsigned char cSignal ) {

    nSignalLength += cMs;

    if( ( cSignal == K_LOW ) && ( nSignalLength > K_SIGNAL_PAUSE ) ) {

        DCF_SignalControl();

        cArrayPointer   = 0;
        nSignalLength   = 0;
    }
}

////////////////////////////////////////////////////////////////////////

void DCF_AnalyseBit( void ) {

    if( nSignalLength < (unsigned char) 150 ) {
        // LOGICAL LOW
        cDCF77[ cArrayPointer ] = 0;
        RS232_WriteString("Received: 0\r\n");
    } else {
        // LOGICAL HIGH
        cDCF77[ cArrayPointer ] = 1;
        RS232_WriteString("Received: 1\r\n");
    }

    DCF_ArrayPointer();

}

////////////////////////////////////////////////////////////////////////

bool DCF_CheckParityPartial(unsigned char iBegin, unsigned char iEnd) {

    unsigned char cOneCount = 0;
    unsigned char cIndex;

    for( cIndex = iBegin; cIndex <= iEnd; cIndex++ ) {
        if( cDCF77[ cIndex ] == 1 ) {
            cOneCount++;
        }
    }

    return (cOneCount%2 == 0)? true : false;
}

////////////////////////////////////////////////////////////////////////

bool DCF_CheckParity(void) {

    // Parity Minute
    if( (bool) cDCF77[28] == DCF_CheckParityPartial(21,27) ) {
        return false;
    }

    RS232_WriteString("Parity Minute: OK\r\n");

    // Parity Hours
    if( (bool) cDCF77[35] == DCF_CheckParityPartial(29,34) ) {
        return false;
    }

    RS232_WriteString("Parity Hours: OK\r\n");

    // Parity Month, Year, Weekday, Day
    if( (bool) cDCF77[58] == DCF_CheckParityPartial(36,57) ) {
        return false;
    }

    RS232_WriteString("Parity M,Y,W,D: OK\r\n");

    return true;
}

////////////////////////////////////////////////////////////////////////

void DCF_SignalControl(void) {

    RS232_WriteString("Pause detected...\r\n");

    RS232_WriteString("Bitcount: ");
    RS232_PrintNumber((unsigned int) cArrayPointer);
    RS232_WriteString("\r\n");

    bSignalError = true;

    if( cArrayPointer == K_DCF77_VALUES ) {

        RS232_WriteString("Received 59 Bits\r\n");
        // DataPackage complete
        if(!DCF_CheckParity()) {
            // Parity check failed.
            return;
        }

        // DataPackage valid.
        bSignalError = false;

        RS232_WriteString("Converting to RAW\r\n");
        DCF_ConvertRaw();

    } else {
        RS232_WriteString("Wrong amount of Bits\r\n");
        return;
    }

    if(bSignalError){
        RS232_WriteString("Some error is occured.\r\n");
    }
    

}

////////////////////////////////////////////////////////////////////////

void DCF_ConvertRaw( void ) {

    tTime.ucMinute      = DCF_BcdToDec(21,27);
    tTime.ucHour        = DCF_BcdToDec(29,34);
    tTime.ucDay         = DCF_BcdToDec(36,41);
    tTime.ucMonth       = DCF_BcdToDec(45,49);
    tTime.ucYear        = DCF_BcdToDec(50,57);
    tTime.ucWeekDay     = DCF_BcdToDec(42,44);

    RS232_WriteString("Aktuelle Uhrzeit/Datum: \r\n");

    RS232_PrintNumber((unsigned int) tTime.ucHour);
    RS232_WriteString(":");

    RS232_PrintNumber((unsigned int) tTime.ucMinute);
    RS232_WriteString(":");

    RS232_WriteString("00 - ");

    RS232_PrintNumber((unsigned int) tTime.ucDay);
    RS232_WriteString(".");
    RS232_PrintNumber((unsigned int) tTime.ucMonth);
    RS232_WriteString(".200");
    RS232_PrintNumber((unsigned int) tTime.ucYear);
    RS232_WriteString(" Wochentag:");

    RS232_PrintNumber((unsigned int) tTime.ucWeekDay);
    RS232_WriteString("\r\n");
}

////////////////////////////////////////////////////////////////////////

struct tRawTime DCF_GetTime( void ) {
    return tTime;
}

////////////////////////////////////////////////////////////////////////

char DCF_BcdToDec( unsigned char cBegin, unsigned char cEnd ) {

    unsigned char cIterator =   cBegin;
    unsigned char iCounter  =   0;
             char cResult   =   0;

    for( ; cIterator <= cEnd ; cIterator++ ) {

        if( cDCF77[cIterator] == 1 ) {
            cResult += cBCD[ iCounter ];
        }

        iCounter++;
    }

    return cResult;
}

////////////////////////////////////////////////////////////////////////

bool DCF_GetErrorState( void ) {
    return bSignalError;
}

////////////////////////////////////////////////////////////////////////

void DCF_ArrayPointer( void ) {

    if( cArrayPointer < K_DCF77_VALUES ){
        cArrayPointer++;
    } else {
        cArrayPointer = 0;
    }
}

