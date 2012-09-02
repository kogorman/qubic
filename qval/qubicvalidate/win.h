/***************************************************************************
                          win.h  -  description
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
 * \brief Declaration of class win.
 */

#ifndef WIN_H
#define WIN_H

#include "iso.h"

/// An abstraction for a line that wins the game.

/// A "win" is an abstraction for a winning line.  It is represented
/// as 4 integers, which are indexes into a board seen as an array of 64 cells.
/// There are just 4 classes of wins, which are toplogically distinct, but we
/// are not concerned with that except as a convenience during generation of
/// all the wins.
///
/// There is a single collection of all of the wins, used as prototypes for
/// line objects.

class win {
    friend class line;
private:
    int index[4];               ///< The indexes of the 4 points.
    static int nextwin;         ///< The number of distinct wins seen so far.
    static win wins[100];       ///< The collection of all wins.
public:
    /// Return the number of wins collected.
    static int count(){return nextwin;}
    /// Set up the collection of wins.
    static void init();
    /// Construct a win: the first prototype.
    win() {
        index[0]=0;
        index[1]=1;
        index[2]=2;
        index[3]=3;
    }
    /// Construct a win with the given points.
    win(int a, int b, int c, int d) {
        index[0]=a;
        index[1]=b;
        index[2]=c;
        index[3]=d;
    }
    /// Return the ith point in this win.
    int val(int i) {return index[i];}
    /// Test for equality.

    /// The test looks sloppy, but considers any permutation to be equal to any other.
    bool operator==(const win& other) {
        for (int i=0; i<3; i++) {
            if ((index[i] != other.index[0])
                && (index[i] != other.index[1])
                && (index[i] != other.index[2])
                && (index[i] != other.index[3])) return false;
        }
        return true;
    }
    /// Add the win \a a to the static collection.

    /// Recognizes and combines duplicates.
    static int add(const win& a) {
        for (int i=0; i<nextwin; i++) {
            if (wins[i]==a) return i;
        }
        wins[nextwin]=a;
        nextwin++;
        if (nextwin>99) {
            cerr << "Too many winning lines" << endl;
            exit(1);
        }
        return nextwin-1;
    }
    /// Print all wins.
    static void printall(void);
};

/// Apply an iso to a win (generates a different win).
win operator*(iso& i, win& l);
/// Output a description of a win to a stream.
ostream& operator<<(ostream&s, win& what);

#endif
