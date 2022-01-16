/* 
 * File:   Comparator.h
 * Author: quinn
 *
 * Created on December 6, 2021, 1:51 PM
 */

#ifndef COMPARATOR_H
#define	COMPARATOR_H

#include <xc.h>
#include <P24F16KA101.h>

void ComparatorInit();

void __attribute__((interrupt, no_auto_psv)) _CompInterrupt(void);

#endif	/* COMPARATOR_H */

