/***************************************************************************
                          point.h  -  description
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
 * \brief Declaration of class point.
 */

#ifndef POINT_H
#define POINT_H

#include "line.h"

/// An individual playable part of the game arena.

class point {
private:
    char value;         ///< What's on this point.
    line *lines[7];     ///< What lines it belongs to.
public:
    int me;             ///< My content.
    static const int EMPTY=0;
    static const int O=1;
    static const int X=2;
    static const int BIGO=3;
    static const int BIGX=4;
    /// Make an empty point, belonging to no lines.
    point() {
        value = EMPTY;
        lines[0]=lines[1]=lines[2]=lines[3]=lines[4]=lines[5]=lines[6]=(line*)0;
    }
    static void init(); ///< Initialize a point.
    int val() {return value;}   ///< Report the contents of a point.
    /// Place a point on a line
    void onto(line* oneline) {
        for (int i=0; i<7; i++) {
            if (lines[i]==(line*)0) {
                lines[i] = oneline;
                return;
            }
        }
        Assert<bad_init>(NASSERT);
    }
    // take() give() and blank() cannot be inline or there's
    // unresolvable circularity with class line.
    void take();    ///< Take this point for 1st player.
    void give();    ///< Give this point to 2nd player.
    void blank();   ///< Blank this point.
    
    bool is_empty() {return value==EMPTY;}  ///< Is this point empty?
    bool is_mine() {return value==X;}       ///< Is this point an exs (1st player)?
    bool is_theirs() {return value==O;}     ///< Is this point an oh (2nd player)?
    int self() {return me;}                 ///< Identify me (0-63).
    void highlight(void);                   ///< Highlight me.
#ifdef QUBICVALIDATE
    int score();
#endif
};
#endif
