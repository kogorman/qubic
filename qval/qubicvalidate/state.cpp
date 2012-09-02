/***************************************************************************
                          state.cpp  -  description
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
 * \brief Member functions of the state class.
 */


#include "state.h"
#include "line.h"

void
state::initstate(statenum who) {
	switch(who) {
	case NONE:
	  setstate(NONE,5,5, NONE, NONE, NONE, NONE);
		break;
	case OPEN:
		setstate(OPEN,0,0,   X1, NONE,   O1, NONE);
		break;
	case X1:
		setstate(X1,  1,0,   X2, OPEN, X1O1, NONE);
		break;
	case X2:
		setstate(X2,  2,0,   X3,   X1, X2O1, NONE);
		break;
	case X3:
		setstate(X3,  3,0,   X4,   X2, X3O1, NONE);
		break;
	case X4:
		setstate(X4,  4,0, NONE,   X3, NONE, NONE);
		break;
	case O1:
		setstate(O1,  0,1, X1O1, NONE,   O2, OPEN);
		break;
	case O2:
		setstate(O2,  0,2, X1O2, NONE,   O3,   O1);
		break;
	case O3:
		setstate(O3,  0,3, X1O3, NONE,   O4,   O2);
		break;
	case O4:
		setstate(O4,  0,4, NONE, NONE, NONE,   O3);
		break;
	case X1O1:
		setstate(X1O1,1,1, X2O1,   O1, X1O2,   X1);
		break;
	case X1O2:
		setstate(X1O2,1,2, X2O2,   O2, X1O3, X1O1);
		break;
	case X1O3:
		setstate(X1O3,1,3, NONE,   O3, NONE, X1O2);
		break;
	case X2O1:
		setstate(X2O1,2,1, X3O1, X1O1, X2O2,   X2);
		break;
	case X2O2:
		setstate(X2O2,2,2, NONE, X1O2, NONE, X2O1);
		break;
	case X3O1:
		setstate(X3O1,3,1, NONE, X2O1, NONE,   X3);
		break;
	default:
		Assert<bad_arg>(NASSERT);
		break;
	}
}
void
state::dump() {
	cerr << "   I am: " << (int)whoami << endl;
	linkdump();
}
