/***************************************************************************
                          win.cpp  -  description
                             -------------------
    begin                : Mon Apr 30 2001
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
 * \brief Member functions of the win class.
 */

#include "iso.h"
#include "win.h"


int win::nextwin;
win win::wins[100];

/// Prints all of the collected wins.
void
win::printall() {
	for (int i=0; i<nextwin; i++) {
		cout << setw(3) << i << wins[i];
	}
}

/// Multiplies a win by an iso.  Sort of.
win operator*(iso& i, win& l) {
	win r(i.val(l.val(0)),
		i.val(l.val(1)),
		i.val(l.val(2)),
		i.val(l.val(3)));
	return r;
}

/// Sends a description to the given stream \a s.
ostream& operator<<(ostream&s, win& what) {
	s << setw(3) << what.val(0) << setw(3) << what .val(1)
		   << setw(3) << what.val(2) << setw(3) << what.val(3) << endl;
	return s;
}

/// Generate the winning lines from 4 prototypes.
/// We only need the prototypes and the isomorphisms to generate all 76
/// of the winning lines.  Note that iso::add() removes duplicates,
/// so we can afford to be overzealous in generating them.
void
win::init() {
	win main(0,21,42,63);			// a major diagonal
	win flat(0,1,2,3);				// flat, but cuts 2 major diagonals
	win body(16,17,18,19);			// flat without cutting a major diagonal
	win diag(0,5,10,15);			// "minor" diagonal.  All cut 2 majors.

	nextwin = 0;

	for (int i=0; i<iso::nextiso; i++) {
		win::add(iso::isos[i] * main);
		win::add(iso::isos[i] * flat);
		win::add(iso::isos[i] * diag);
		win::add(iso::isos[i] * body);
	}
#ifndef NDEBUG
	cout << "There are " << nextwin << " winning lines" << endl;
#endif
}
