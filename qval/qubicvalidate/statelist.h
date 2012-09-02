/***************************************************************************
                          statelist.h  -  description
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
 * \brief Declaration of class statelist.
 */

#ifndef STATELIST_H
#define STATELIST_H

#include "state.h"
#include "line.h"

/// A list of states.

class statelist {
private:
    state states[state::cardinality];
public:
    /// Constructor to make an initialized list.
    statelist();
    /// Determine if this statelist includes an immediately winnable line (X3).
    bool canwin() {return !states[state::X3].isempty();}
    /// Find the point that can immediately win this board.
    line *theWin() {return (line *)states[state::X3].member();}
    /// Determine if this statelist forces us (O3).
    bool haveforced() {return !states[state::O3].isempty();}
    /// Find the line that the opponent has forced with.
    line *theForce() {return (line *)states[state::O3].member();}
    /// Determine if this statelist includes a line we can force with (X2).
    bool canforce() {return !states[state::X2].isempty();}
    /// Determine if a move is forcing.
    int canForceAt(int *where);
    /// Record the state of a line.
    void makeme(int me, line *l) {
        states[me].add(l);
    }
    /// Enhance the display of any winning line.
    void highlight();
    /// Add a line to a state.
    void add(line *what, state::statenum as) {
        states[as].add(what);
    }
    /// Dump the member lines of a state.
    void dump(state::statenum it) {
        states[it].dump();
    }
};
#endif
