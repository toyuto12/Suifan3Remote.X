#ifndef DMY_H
#define DMY_H

#include <stdbool.h>
#include <stdint.h>

typedef union {
    struct {
        unsigned STR1A                  :1;
        unsigned STR1B                  :1;
        unsigned STR1C                  :1;
        unsigned STR1D                  :1;
        unsigned STR1SYNC               :1;
    };
} PSTR1CONbits_t;

typedef union {
    struct {
        unsigned LATA0                  :1;
        unsigned LATA1                  :1;
        unsigned LATA2                  :1;
        unsigned                        :1;
        unsigned LATA4                  :1;
        unsigned LATA5                  :1;
    };
} LATAbits_t;

#ifdef TDD_BASE
    PSTR1CONbits_t PSTR1CONbits;
    LATAbits_t LATAbits;
#else
    extern PSTR1CONbits_t PSTR1CONbits;
    extern LATAbits_t LATAbits;
#endif
#endif

