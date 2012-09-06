/***************************************************************************
                          line.h  -  description
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
 * \brief Declaration of class line.
 */

#ifndef LINE_H
#define LINE_H

class line;

#include "win.h"
#include "point.h"
#include "llink.h"
#include "state.h"

class point;
class board;

/// A set of 4 points, comprising a possible winning set of points.

/// A line is an example of a win, but not a subclass of that class.
/// It represents the 'win' in an actual board, which some actual collection
/// of 4 points.

class line: public llink {
    friend class win;
private:
    int me;             //!< \brief My line number.
    board *whose;       //!< \brief What board I belong to
    win* theWin;        //!< \brief The win corresponding to this line
    point* values[4];   //!< \brief The points comprising this line
    int Xs, Os;         //!< \brief Counters for the state of the line
    void makeme(int what);
public:
    /// linkup makes the line correspond to points in its board.
    void linkup(int whatwin, point *points, board *myBoard);
    int self() {return me;}     //!< \brief Integer value of this.
    state * mylist() {return (state *) myhead();}
    void dump();    //!< \brief Output this state.
    /// Add an ex to this state.
    void addx() {
        Xs++;
        makeme(mylist()->Addx);
    }
    /// Subtract an ex from this state.
    void subx() {
        Xs--;
        makeme(mylist()->Dropx);
    }
    /// Add an oh to this state.
    void addo() {
        Os++;
        makeme(mylist()->Addo);
    }
    /// Subtract an oh from this state
    void subo() {
        Os--;
        makeme(mylist()->Dropo);
    }
    /// An open point on a line with this state.
    int openPoint(void);
    /// How many open points on this line
    int openPoints(int, int*);
    int addPoint(int, int*, int);
    /// Highlight the points on this line (because it won).
    void highlight(void);
#ifdef QUBICVALIDATE
    /// How many exs on this line.
    int Xcount() {return Xs;}
    /// How many ohs on this line.
    int Ocount() {return Os;}
#endif
};

#endif

