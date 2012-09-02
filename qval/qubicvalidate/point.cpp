/***************************************************************************
                          point.cpp  -  description
                             -------------------
    begin                : Tue May 1 2001
    copyright            : (C) 2001 by Kevin O'Gorman
    email                : kogorman@kosmanor.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License v2, as published *
 *   by the Free Software Foundation.                                      *
 *                                                                         *
 ***************************************************************************/

/*! \file
 * \brief Member functions of the point class.
 */

#include "qval.h"
#include "point.h"

void
point::init() {
	
}

void
point::take() {
	Assert<bad_move>(NASSERT || value==EMPTY);
	for (int i=0; i<7; i++) {
		if (lines[i]==(line*)0) break;
		lines[i]->addx();
	}
	value = X;
}

void
point::give() {
	Assert<bad_move>(NASSERT || value==EMPTY);
	for (int i=0; i<7; i++) {
		if (lines[i]==(line*)0) break;
		lines[i]->addo();
	}
	value = O;
}
	
void
point::blank() {
	Assert<bad_move>(NASSERT || value != EMPTY);
	for (int i=0; i<7; i++) {
		if (lines[i]==(line*)0) break;
		if (value == O || value==BIGO) {
			lines[i]->subo();
		} else if (value == X || value==BIGX) {
			lines[i]->subx();
		} else {
			Assert<bad_value>(NASSERT);
		}
	}
	value = EMPTY;
}

void
point::highlight() {
	Assert<bad_move>(NASSERT || value!=EMPTY);
	if (value==O) value=BIGO;
	else if (value==X) value=BIGX;
}

#ifdef QUBICVALIDATE
int
point::score() {
	int s = 0;
	for (int i=0; i<7; i++) {
		if (lines[i] == (line*)0) break;
		if (lines[i]->Ocount() != 0) continue;
		// A single point with two threats is a winner
		// but even one is very good,
		// and even opening a line is nice
		s += (1 << (3 * (lines[i]->Xcount())));
	}
	return s;
}
#endif
