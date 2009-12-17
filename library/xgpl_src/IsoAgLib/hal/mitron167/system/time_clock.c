/*
  time_clock.c

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
// Adaptations for mitron with C167
#define USE_T6_TIMER
#define LONGPERIOD

#ifndef CPUTYPE
  #define CPUTYPE 0x167
#endif

#ifndef HASBCLKCON
#ifdef CPUTYPE
#if CPUTYPE == 0x164
#define HASBCLKCON	0
#undef USE_T6_TIMER
#endif	/* CPUTYPE == 0x164 */
#if CPUTYPE == 0x166
#define HASBCLKCON	0
#endif	/* CPUTYPE == 0x166 */
#if CPUTYPE == 0x167
#define HASBCLKCON	0
#endif	/* CPUTYPE == 0x167 */
#if CPUTYPE == 0x1661
#define HASBCLKCON	0
#endif	/* CPUTYPE == 0x1661 */
#if CPUTYPE == 0x1662
#define HASBCLKCON	1
#endif	/* CPUTYPE == 0x1662 */
#endif	/* CPUTYPE */
#endif	/* HASBCLKCON */

#ifndef HASBCLKCON
#define HASBCLKCON	0
#endif	/* HASBCLKCON */

#if HASBCLKCON
#include <regsuper10bo.h>
#else	/* HASBCLKCON */
#include <reg166.h>
#endif	/* HASBCLKCON */
#include <stddef.h>
#include <time.h>

#if HASBCLKCON
/* A Super10/C166SV2 with 96MHz CPU frequency is assumed.
 * Change the values if needed.
 * The following formulae can be used:
 * f_T3 = CPU_FREQ/(PRESC_BCLKCON*PRESC_BPS1*pow(2,T3I))
 * f_T6 = CPU_FREQ/(PRESC_BCLKCON*PRESC_BPS2*pow(2,T6I))
 * !!! with same bitpattern: PRESC_BPS1 = 2 * PRESC_BPS2 !!!
 */
#define OTLVAL	0x0400		/* TxCON.TxOTL: 1 */
#ifdef USE_T6_TIMER
#define BPSVAL	0x0000		/* T6CON.BPS2: PRESC_BPS2 = 4 */
#else	/* USE_T6_TIMER */
#define BPSVAL	0x0800		/* T3CON.BPS1: PRESC_BPS1 = 4 */
#endif	/* USE_T6_TIMER */
#ifndef LONGPERIOD	/* PRESC_BCLKCON is assumed to be 8 so the highest safe TxI prescaler */
			/* value is 16 to be able to adjust TxI for other PRESC_BCLKCON values */
#define INPUTFRQ	375	/* resolution 2.67us, max 3h10m53s, error 0% */
#define PRESCTXI	3	/* TxI prescaler factor 8 */
#else	/* LONGPERIOD */
#define INPUTFRQ	187	/* resolution 5.33us, max 6h21m46s, error 0.27% */
#define PRESCTXI	4	/* TxI prescaler factor 16 */
#endif	/* LONGPERIOD */
#else	/* HASBCLKCON */
/* A 166/167 with 20MHz CPU frequency is assumed.
 * Change the values if needed.
 * The following formulae can be used:
 * f_T3 = CPU_FREQ/(8*pow(2,T3I))
 * f_T6 = CPU_FREQ/(4*pow(2,T6I))
 */
#define BPSVAL	0x0000		/* No TxCON.BPSn field */
#ifndef LONGPERIOD
  #define INPUTFRQ	625	/* resolution 1.60us, max 1h54m31s, error 0% */
  #ifdef USE_T6_TIMER
    #define OTLVAL	0x0400		/* T6CON.T6OTL: 1 */
    #define	PRESCTXI	3	/* T6I prescaler factor 8 */
  #else	/* USE_T6_TIMER */
    #define OTLVAL	0x0000		/* No T3CON.T3OTL */
    #define PRESCTXI	2	/* T3I prescaler factor 4 */
  #endif	/* USE_T6_TIMER */
#else	/* LONGPERIOD */
  #define INPUTFRQ	39	/* resolution 25.60us, max 30h32m31s, error 0.16% */
  #define OTLVAL	0x0000		/* No T3CON.T3OTL */
  #ifdef USE_T6_TIMER
    #define PRESCTXI	7	/* T6I prescaler factor 128 */
  #else	/* USE_T6_TIMER */
    #define PRESCTXI	6	/* T3I prescaler factor 64 */
  #endif	/* USE_T6_TIMER */
#endif	/* LONGPERIOD */
#endif	/* HASBCLKCON */

/* Select T3 or T6.
 * T3 is the default now because the C164 does not have
 * the GPT2 timers (T5 and T6).
 */
#ifdef USE_T6_TIMER
#define TIMERCOUNT	T6	/* counter register */
#define TIMERHIGH	T5	/* high word counter register */
#define TIMERCON	T6CON	/* timer config register */
#define TIMERHCON	T5CON	/* counter config register */
#else	/* USE_T6_TIMER */
#define TIMERCOUNT	T3
#define TIMERHIGH	T2
#define TIMERCON	T3CON
#define TIMERHCON	T2CON
#endif	/* USE_T6_TIMER */

#define TXR		0x0040		/* TxR bit */
#define TIMERCONINIT	OTLVAL|BPSVAL|TXR|PRESCTXI	/* TxR, timer mode, count up */
#define TIMERHCONINIT	0x004f		/* counter mode */
#define TIMERCONMASK	0x00ff		/* for running timer detection */
/* Return number of milliseconds since first call. If the timer doesn't
 * run yet it is started.
 */
time_t _time( time_t *pt )
{
	time_t		Cnt;

	if ( (TIMERHCON & TIMERCONMASK) != (TIMERHCONINIT & TIMERCONMASK ) )
	{
#if HASBCLKCON
		unsigned int bclkcon = (SYSCON1 >> 8) & 0x0f;
#endif	/* HASBCLKCON */
		TIMERCON = TIMERCONINIT & ~TXR; /* setup timer but don't run	*/
		TIMERHCON = TIMERHCONINIT;	/* setup counter */
		TIMERCOUNT = 0;			/* clear counter */
		TIMERHIGH = 0;
#if HASBCLKCON
		/* Adjust TxI for PRESC_BCLKCON values unequal to 8:
		 * BCLKCON=0x0 (PRESC_BCLKCON=8): TxI doesn't change
		 * BCLKCON=0x1 (PRESC_BCLKCON=1): TxI+=3 (extra 8x frq division)
		 * BCLKCON=0x2 (PRESC_BCLKCON=2): TxI+=2 (extra 4x frq division)
		 * BCLKCON=0x3 (PRESC_BCLKCON=4): TxI+=1 (extra 2x frq division)
		 */
		if ( bclkcon )
		{
			TIMERCON += 4 - bclkcon;
		}
#endif	/* HASBCLKCON */
		if ( pt )
		{
		    *pt = (time_t)0;
		}
		TIMERCON |= TXR;		/* timer runs */
		return ( (time_t) 0 );
	}

	do	/* Be sure TIMERHIGH is not updated after we have read it */
	{
		Cnt = ((time_t)TIMERHIGH << 16) | TIMERCOUNT;
	} while( (unsigned)(Cnt >> 16) != TIMERHIGH );

	Cnt /= INPUTFRQ;	/* elapsed ms units      */

	if ( pt )
	{
		*pt = Cnt;
	}
	return ( Cnt );
}


