#ifndef TDD
#include "mcc_generated_files/mcc.h"
#endif

typedef union{
	uint32_t dat32;
	uint8_t dat8[4];
}stSendData;

uint8_t gInterval;
uint8_t sIrHighWidth,sIrLowWidth;
stSendData sIrBuf;
uint8_t sIrBufPos,sIrSendTime,sIrEndBit;


// 424usInterval
#define INTERVAL_10MS	24
void isrTimer0(void){
	gInterval ++;

	// Create IrPulse
	if( sIrSendTime ) sIrSendTime --;
	if( !sIrLowWidth ){
		// BitDataExchange
		if( sIrBufPos ){				// 
			sIrBufPos --;
			sIrLowWidth = ( sIrBuf.dat8[0] & 0x01 ) ?3 :1 ;
			sIrBuf.dat32 >>= 1;
			PSTR1CONbits.STR1A = 1;
		}else{							// 
			PSTR1CONbits.STR1A = sIrEndBit;
			LATAbits.LATA5 = 0;
			sIrEndBit = 0;
		}
	}else if( sIrHighWidth ){
		sIrHighWidth --;
		PSTR1CONbits.STR1A = 1;
	}else{
		sIrLowWidth --;
		PSTR1CONbits.STR1A = 0;
		LATAbits.LATA5 = 0;
	}
}

#define IsSendIrEnable	((sIrSendTime) ?0 :1)

void SendIr( uint8_t customCode, uint8_t dat){
	sIrBuf.dat8[0] = customCode;
	sIrBuf.dat8[1] = ~customCode;
	sIrBuf.dat8[2] = dat;
	sIrBuf.dat8[3] = ~dat;
	
	sIrHighWidth = 16;
	sIrLowWidth = 8;
	sIrBufPos = 32;
	sIrSendTime = 255;
	sIrEndBit = 1;
}

// Code
// 0 : None
// 1 : MainFanOn
// 2 : MainFanMove
// 3 : Timer
// 4 : LeftRightFan
// 5 : UpDownFan
#ifndef TDD

#define iSW_MAIN_ON		!PORTAbits.RA0
#define iSW_TIMER		!PORTAbits.RA1
#define iSW_MAIN_MOVE	!PORTAbits.RA2
#define iSW_UDFAN		!PORTAbits.RA3
#define iSW_LRFAN		!PORTAbits.RA4
uint8_t TaskInputCode( void ){
	static uint8_t tim,exCode;
	uint8_t code = 0;

	if( iSW_LRFAN ) code = 4;
	if( iSW_TIMER ) code = 3;
	if( iSW_MAIN_MOVE ) code = 2;
	if( iSW_MAIN_ON ) code = 1;
	if( iSW_UDFAN ) code = 5;
	
	if( exCode == code ) tim = 0;
	else if( tim > 100 ) tim ++;
	else exCode = code;
	
	return exCode;
}

void main(void){

	SYSTEM_Initialize();
	TMR0_SetInterruptHandler(isrTimer0);
	
	PSTR1CONbits.STR1A = 0;
	TMR2_StartTimer();
	
	INTERRUPT_GlobalInterruptEnable();
	INTERRUPT_PeripheralInterruptEnable();

    while (1){

		// 10msInterval
		if( gInterval >= INTERVAL_10MS ){
			gInterval -= INTERVAL_10MS;
			
			uint8_t code = TaskInputCode();
//			if( code && IsSendIrEnable ){
			if( IsSendIrEnable ){
				SendIr( 0x93, code );
			}
		}
	}
}
#endif
