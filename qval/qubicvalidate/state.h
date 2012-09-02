/***************************************************************************
                          state.h  -  description
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
 * \brief Declaration of class state.
 */

#ifndef STATE_H
#define STATE_H

#include "llink.h"

/// A state which may describe the condition of a line on the board.

/// Each line of a board can be in one of a number of states, depending on how
/// many Xs and Os it has.  Lines start in state OPEN and may move up in both
/// Xs and Os as the game progresses.  By definition, any line entering state
/// X4 or O4 ends the game.  If we're doing this right, it will always be X4.

class state: private llink {
friend class line;
friend class statelist;
private:
    /// An enum that enumerates the competetive states of a line on the board.
    /*! These are mostly named by the number of x's and o's in the line, and
     * include all possibilites that can occur.
     */
    enum statenum {NONE=0,OPEN,X1,X2,X3,X4,O1,O2,O3,O4,X1O1,X1O2,X1O3,X2O1,X2O2,X3O1};
    /// The number of different statenums that exist.
    /*! This is useful for the statelist class, which contains mostly a count
     * of the number of points in each state.
     */
    static const int cardinality = X3O1+1;  ///< Cardinality of the set of states.
    statenum whoami;        ///< The number of this state.
    int    Xs;           ///< Count of exs.
    int    Os;           ///< Count of ohs.
    statenum Addx;      ///< The state plus one exs.
    statenum Dropx;      ///< The state minus one exs.
    statenum Addo;      ///< The state plus one oh.
    statenum Dropo;      ///< The state minus one oh.
    /// Set the state according to the given statistics.
    void setstate(statenum who, int xs, int os, statenum addx, statenum dropx,
            statenum addo, statenum dropo){
        makehead();                    // The state is a head of a list
        whoami = who;
        Xs     = xs;
        Os     = os;
        Addx   = addx;
        Dropx  = dropx;
        Addo   = addo;
        Dropo  = dropo;
    }
    void initstate(statenum who);
public:
    bool isblocked(){return Xs>0 && Os>0;}      ///< Do both players own a piece of it?
    bool isforcing() {return Xs==3 && Os==0;}   ///< Are there 3 exs, no ohs?
    bool canforcing() {return Xs==2 && Os==0;}  ///< Are there 2 exs, no ohs?
    bool canforced() {return Xs==0 && Os==2;}   ///< Are there 2 ohs, no exs?
    bool isforced() {return Xs==0 && Os==3;}    ///< Are there 3 ohs, no exs?
    bool isgood() {return Xs>0 && Os ==0;}      ///< Are there any exs, no ohs?
    bool isbad() {return Xs==0 && Os > 0;}      ///< Are there any ohs, no exs?
    void dump(void);                            ///< Output.
    int mystate() {return whoami;}              ///< Return the number of this state.
};

#endif
