/*
 * File:   uart.c
 * Author: lal
 *
 * Created on 9 avril 2015, 07:44
 *
 * modified by JMC
 * - TxD line declared as input in routine UARTInit, to be compliant with datasheet
 * - in Interrupt Service Routine SerialRxPinInterrupt(), line PIR1bits.RCIF = 0; is commented since it is useless
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "general.h"
#include "lcd.h"
#include "uart.h"


void UARTInit(UINT16_T baudRate) {

    TRISCbits.TRISC6 = INP;                     // RC6 is TxD (but should be declared first as input)
    TRISCbits.TRISC7 = INP;                     // RC7 is RxD
    ANSELCbits.ANSC7 = DISABLE;                 // disable analog input

    TXSTA1 = CLEAR;                             // reset USART registers
    RCSTA1 = CLEAR;

    TXSTA1bits.SYNC = CLEAR;                    // asynchronous mode
    TXSTA1bits.TX9 = CLEAR;                     // 8-bit mode
    RCSTA1bits.RX9 = CLEAR;
    RCSTA1bits.CREN = SET;                      // continous receive
    RCSTA1bits.ADDEN = SET;                     // address detect enable
    PIE1bits.RC1IE = SET;                       // interrupt on receive
    PIE1bits.TX1IE = CLEAR;                     // no interrupt on transmission

    TXSTA1bits.BRGH = SET;                      // baud rate select (high speed)
    BAUDCON1bits.BRG16 = SET;                   // see PIC18F44K22 Silicon Errata (5. Module: EUSART, page 2)
    SPBRG1 = (UINT8_T)(((_XTAL_FREQ / (4 * baudRate)) - 1));   // = 25
    SPBRGH1 = 0;

//    SPBRG1 = (UINT8_T)((_XTAL_FREQ / baudRate) / 16 - 1);   // set baud rate divider

    TXSTA1bits.TXEN = ENABLE;                   // enable transmitter
    RCSTA1bits.SPEN = ENABLE;                   // enable receiver

    RCIF = CLEAR;                               // reset RX pin flag
    RCIP = CLEAR;                               // not high priority
    RCIE = ENABLE;                              // enable RX interrupt
    PEIE = ENABLE;                              // enable pheripheral interrupt (serial port is a pheripheral)
}

void UARTWriteByte(UINT8_T data) {

    while (TXSTA1bits.TRMT == CLEAR);           // wait for transmission idle

    TXREG1 = data;
}

UINT8_T UARTReadByte(void) {

    return(RCREG1);
}

void __interrupt() SerialRxPinInterrupt() {

    if (PIR1bits.RCIF == 1) {               //check if the interrupt is caused by RX pin

        if (RCSTA1bits.FERR) {
            LCDGoto(0, 1);
            LCDWriteStr("Framing Error");
            exit(EXIT_FAILURE);
        } else if (RCSTA1bits.OERR) {
            LCDGoto(0, 1);
            LCDWriteStr("Overrun Error");
            exit(EXIT_FAILURE);
        } else {
            LCDPutChar(UARTReadByte());     // display the received character
            // PIR1bits.RCIF = 0;              // clear rx flag - useless: flag RCIF is cleared when RX buffer is read
        }
    }
}


