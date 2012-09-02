/***************************************************************************
                          strategic.h  -  description
                             -------------------
    begin                : Wed Jun 6 2001
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
 * \brief Declaration of class strategic.
 */

#ifndef STRATEGIC_H
#define STRATEGIC_H

/// A "strategic" position in the strategy, and corresponding chosen play.
class strategic {
    friend class board;
private:
    static int i;   ///< The count of strategic moves.
    static void makeNext(char *pattern, int mt);    ///< Add one to the collection.
    static strategic smv[2929]; ///< The array of all strategic moves.
public:
    movenum points[64];     ///< The canonic board for this position
    movenum moveto;         ///< Where the 1st player can win
    char *pattern;          ///< The text representation of this position.
    static void init();     ///< Initialize.
    /// For validation -- produce the indexed strategic object.
    static strategic &find(int i) {
        return smv[i];
    }
};

#endif
