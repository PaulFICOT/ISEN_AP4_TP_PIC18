/*
 * File:   main.c
 * Author: paul
 *
 * Created on February 5, 2021, 3:48 PM
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "general.h"
#include "lcd.h"
#include "leds.h"

// PIC18F46K22 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config FOSC = INTIO7    // Oscillator Selection bits (Internal oscillator block, CLKOUT function on OSC2)
#pragma config PLLCFG = OFF     // 4X PLL Enable (Oscillator used directly)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock is always enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power-up Timer Enable bit (Power up timer disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON     // PORTB A/D Enable bit (PORTB<5:0> pins are configured as digital I/O on Reset)
#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
#pragma config P2BMX = PORTD2   // ECCP2 B output mux bit (P2B is on RD2)
#pragma config MCLRE = EXTMCLR  // MCLR Pin Enable bit (MCLR pin enabled, RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection Block 0 (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection Block 1 (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection Block 2 (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection Block 3 (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define BLANCHE 0
#define NOIRE 1

//Frequence des notes
float do2=261.63;
float re2=293.66;
float mi2=329.63;
float fa2=349.23;
float sol2=392;
float la2=440;
float si2=493.88;

void note(float note){
    //Calcul du PR2 à partir de la frequence
    PR2 =(float) (1000000 / (note * 4 * 16)) + 1;
    //Calcul rapport cyclique
    CCPR1L = PR2/2;
}

void timerNote(int high, int low) {
    T0CON = 0x00;
    TMR0H = high;
    TMR0L = low;
    T0CON = 0x82;
    while(TMR0IF != 1) {
        Nop();
    }
    TMR0IF = 0;
}

void delayNote(int isBlack) {
    if(isBlack == TRUE) {
        //0.75SEC
        timerNote(0xD2, 0x39);
    }
    else {
        //1.5SEC
        timerNote(0xA4, 0x72);
    }
}

void main(void) {
    //Ordre des frequences des notes sur la partition
    float frequenceMusique[] = {do2, do2, sol2, sol2, la2, la2, sol2, fa2, fa2, mi2, mi2, re2, re2, do2, sol2, sol2, fa2, fa2, mi2, mi2, mi2, re2, sol2, sol2, fa2, fa2, mi2, mi2, mi2, re2, do2, do2, sol2, sol2, la2, la2, sol2, fa2, fa2, mi2, mi2, re2, re2, do2};

    //Ordre des paroles en fonction des notes
    char* parolesMusique[] = {"Ah! ", "vous ", "di", "rai-", "je, ", "ma", "man, ","ce ","qui ","cau","se ","mon ","tour","ment! ","Pa","pa ","veut ", "que ","je ", "rai", "son", "ne ", "comme ", "u", "ne ", "gran", "de ", "per", "son", "ne; ", "Moi, ", "je ", "dis ", "que ", "les ", "bon", "bons ", "va", "lent ", "mieux ", "que ", "la ", "rai", "son."};

    //Ordre des types de notes sur la partition
    int notesMusique[] = {NOIRE, NOIRE, NOIRE, NOIRE, NOIRE, NOIRE, BLANCHE, NOIRE, NOIRE, NOIRE, NOIRE ,NOIRE ,NOIRE, BLANCHE, NOIRE, NOIRE, NOIRE, NOIRE, NOIRE, NOIRE, NOIRE, NOIRE ,NOIRE ,NOIRE ,NOIRE ,NOIRE ,NOIRE ,NOIRE ,NOIRE ,NOIRE ,NOIRE, NOIRE ,NOIRE ,NOIRE ,NOIRE ,NOIRE ,NOIRE ,BLANCHE, NOIRE, NOIRE, NOIRE, NOIRE, NOIRE, NOIRE, BLANCHE};
    
    //Taille du tableau
    char musiqueSize = sizeof (frequenceMusique);
    
    //Variable LED chenillard
    int led = 0;

    //Variable LCD Shifting
    int lcd_length = 0;
    
    //
    TRISB = OUTP;
    
    //Buzzer output
    TRISCbits.TRISC2 = OUTP;
    
    T2CON = 0x06;
    CCP1CON = 0x0C;
    
    LCDInit();
    
    for(int i = 0; i < musiqueSize; i++) {
        lcd_length += strlen(parolesMusique[i]);
        if(lcd_length > 40) {
            LCDClear();
            lcd_length -= 40;
        } else {
            if(lcd_length > 16) {
                LCDShiftDisplay(strlen(parolesMusique[i]));
            }
        }
        LCDWriteStr(parolesMusique[i]);
        note(frequenceMusique[i]);
        switch(led) {
            case 0: 
                LATB = 0x01;
                led = 1;
                break;
            case 1:
                LATB = 0x02;
                led = 2;
                break;
            case 2:
                LATB = 0x04;
                led = 3;
                break;
            case 3:
                LATB = 0x08;
                led = 0;
                break;
        }
        delayNote(notesMusique[i]);
        PR2 = 0;
    }
    return;
}
