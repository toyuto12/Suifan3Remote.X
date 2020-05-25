#include <stdbool.h>
#include <stdint.h>

#define EQUAL cut_assert_equal_int

typedef union{
	uint32_t dat32;
	uint8_t dat8[4];
}stSendData;
extern PSTR1CONbits_t PSTR1CONbits;
extern stSendData sIrBuf;
void isrTimer0(void);
void SendIr( uint8_t customCode, uint8_t dat);

void _checkIrHeader(){
    uint8_t cnt = 16,r=true;
    while( cnt-- ){
        isrTimer0();
        if( !PSTR1CONbits.STR1A ) cut_error( "NotHigh:%d", 16-cnt );
    }
    cnt = 8;
    while( cnt-- ){
        isrTimer0();
        if( PSTR1CONbits.STR1A ) cut_error( "NotLow:%d", 8-cnt );
    }
    isrTimer0();
    cut_assert_true( PSTR1CONbits.STR1A );
}

uint8_t _checkBit(){
    uint8_t cnt = 0;
    isrTimer0();
    while( !PSTR1CONbits.STR1A ){
        cnt ++;
        isrTimer0();
    }
    switch( cnt ){
        case 1: return 0;
        case 3: return 1;
        default: return -1;
    }
}

uint8_t _getChar(){
    uint8_t cnt = 8,r,tmp;
    while( cnt-- ){
        tmp = _checkBit();
        if( tmp == -1 ) cut_error( "bitError:%d",cnt-8 );
        else r = (tmp ?0x80 :0x00) | (r>>1);
    }
    return r;
}

extern uint8_t sIrSendTime;
#define IsSendIrEnable	((sIrSendTime) ?0 :1)
void test_SendIr(){
    uint8_t head=0x12,data=0x34;
    SendIr( head, data );       // Data“o˜^
    _checkIrHeader();
    EQUAL( head, _getChar() );
    EQUAL( 0xFF, head + _getChar() );
    EQUAL( data, _getChar() );
    EQUAL( 0xFF, data + _getChar() );

    uint16_t cnt=0;
    while( !IsSendIrEnable ){
        isrTimer0();
        if( PSTR1CONbits.STR1A ) cut_error( "CommLineError" );
        cnt ++;
    }
    cut_assert( cnt > 100 );
}
