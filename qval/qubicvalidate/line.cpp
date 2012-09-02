/***************************************************************************
                          line.cpp  -  description
                             -------------------
    begin                : Sat Apr 28 2001
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
 * \brief Member functions of class line.
 */

#include "line.h"
#include "board.h"

void
line::makeme(int what) {
		drop();
		whose->status.makeme(what, this);
}

void
line::linkup(int whatwin, point *points, board *myBoard) {
	whose = myBoard;
	me = whatwin;
	theWin = &win::wins[whatwin];
	values[0] = &points[theWin->index[0]];
	points[theWin->index[0]].onto(this);
	values[1] = &points[theWin->index[1]];
	points[theWin->index[1]].onto(this);
	values[2] = &points[theWin->index[2]];
	points[theWin->index[2]].onto(this);
	values[3] = &points[theWin->index[3]];
	points[theWin->index[3]].onto(this);
	Xs = Os = 0;
	try {
		myBoard->status.add(this,state::OPEN);
	} catch(bad_list &e) {
		cerr << endl << "BAD_LIST in line::linkup" << endl;
		cerr << "LINE:" << endl;
		this->dump();
		cerr << "State: (" << (int)state::OPEN << ")" << endl;
		myBoard->status.dump(state::OPEN);
		throw;
	}
}

void
line::dump() {
	cerr << "Self: " << self() << endl;
	cerr << "Xs: " << Xs << endl;
	cerr << "Os: " << Os << endl;
	cerr << "points: ";
		for (int i=0; i<4; i++) {
			cerr << " " << values[i]->self();
		}
	cerr << endl;
}

int
line::openPoint() {
	if (values[0]->val() == point::EMPTY) {return values[0]->me;}
	else if (values[1]->val() == point::EMPTY) {return values[1]->me;}
	else if (values[2]->val() == point::EMPTY) {return values[2]->me;}
	else if (values[3]->val() == point::EMPTY) {return values[3]->me;}
	Assert<bad_result>(NASSERT);
	return -1;
}

int
line::addPoint(int at, int *f, int m) {
	int i;
	for (i=0; i<at; i++) {
		if (f[i] == m) return at;
	}
	f[at++] = m;
	return at;
}

int
line::openPoints(int at, int *f) {
	if (values[0]->val() == point::EMPTY) {at = addPoint(at,f,values[0]->me);}
	if (values[1]->val() == point::EMPTY) {at = addPoint(at,f,values[1]->me);}
	if (values[2]->val() == point::EMPTY) {at = addPoint(at,f,values[2]->me);}
	if (values[3]->val() == point::EMPTY) {at = addPoint(at,f,values[3]->me);}
	return at;
}

void
line::highlight() {
	values[0]->highlight();
	values[1]->highlight();
	values[2]->highlight();
	values[3]->highlight();
}	
