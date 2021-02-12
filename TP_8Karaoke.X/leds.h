/*
 * File:   leds.h
 * Author: lal
 *
 * Created on 1 avril 2015, 09:39
 */

#ifndef _LEDS_H
#define	_LEDS_H

#define LED0_DIR	TRISBbits.TRISB0	// direction of RB0 (OUTP)
#define LED1_DIR	TRISBbits.TRISB1	// direction of RB1 (OUTP)
#define LED2_DIR	TRISBbits.TRISB2	// direction of RB2 (OUTP)
#define LED3_DIR	TRISBbits.TRISB3	// direction of RB3 (OUTP)

#define LED0_STATE	LATBbits.LATB0		// state of led0 (ON/OFF)
#define LED1_STATE	LATBbits.LATB1		// state of led1 (ON/OFF)
#define LED2_STATE	LATBbits.LATB2		// state of led2 (ON/OFF)
#define LED3_STATE	LATBbits.LATB3		// state of led3 (ON/OFF)

#endif /* _LEDS_H */

