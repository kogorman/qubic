/***************************************************************************
                          statelist.cpp  -  description
                             -------------------
    begin                : Sat Jul 21 2001
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
 * \brief Member functions of the statelist class.
 */

#include "board.h"
#include "statelist.h"

statelist::statelist(void) {
    for (int i=0; i<state::cardinality; i++) {
        states[i].initstate((state::statenum)i);
    }
}


// Return a list (in f[]) of forcing points
int
statelist::canForceAt(int *f) {
    llink *base, *obase;
    int at=0;
    
    obase = base = &states[state::X2];
    while ((base = base->next()) != obase) {
        at = ((line *)base)-> openPoints(at,f);
    }
    f[at] = -1;
    return at;
}


// void statelist::highlight()
//  Used when the game is over to highlight the
//  winning line(s).  Called only for O4 and X4
//  states.
void
statelist::highlight() {
    llink *cursor, *base;
    
    base = cursor = &states[state::X4];
    while ((cursor=cursor->next()) != base) {
        ((line*)cursor)->highlight();
    }
    
    base = cursor = &states[state::O4];
    while ((cursor=cursor->next()) != base) {
        ((line*)cursor)->highlight();
    }
}
