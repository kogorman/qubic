/***************************************************************************
                          board.h  -  description
                             -------------------
    begin                : Wed May 2 2001
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
 * \brief Declaration of the board class.
 *
 * Contains some member definitions, if they are short and suitable for
 * inline code generation.  A few of them are conditional, and may appear
 * in the file board.cpp instead.
 */
#ifndef BOARD_H
#define BOARD_H

#include "qval.h"
#include "strategic.h"

#include "point.h"
#include "line.h"
#include "win.h"
#include "iso.h"
#include "statelist.h"

/// The game arena.
/**
 * The board is represented as an array of 64 point objects.  This is just
 * a 1-dimensional list, so the true topology of the %board is represented
 * by grouping the points in line objects, each representing a way to win
 * the game.
 */

class board {
friend class line;
private:
    point points[64];
    line  lines[76];
    int plays;
    movenum moves[64];
    int forcing;            //!< move that started forcing sequence.
    iso *canonical();
    //! A representation of the result of branch-and-bound search.
    /**
     * There's a two-part result: the move to make to get the best result, and
     * a depth, which is the length of the sequence that acheives the
     * result.  For a good result, shallow depths are better, for an
     * unfavorable one, deep is better (it delays the undesirable).
     */
    typedef struct {
        int where;      //!< The move to make for the result.
        int depth;      //!< The depth achieved.
    } bound;

    //! \brief Determine if there's a winning sequence of forces.
    bound sequence(int b);
    bound willwin(int b);
    int trim();             //!< removes unneeded moves
    int itrim(int,int);     //!< used internal to trim()
    statelist status;       //!< census of each line state
    int seqlevel;
    int seqboards;
    bool haveSolution;
public:
    board():status() {init();}  //!< \brief Construct and initialize
    void init();                //!< \brief Initialize the game arena.
    void show(const iso &v);    //!< \brief Show the board through a particular iso.
    //! Show the board in its true form.

    /// For this purpose
    /// the identity isomorphism is used.
    void show() {show(iso::isos[0]);}
    //! Show the board in its canonic form.
    void showcanonic() {
            show(*canonical());
        }
    movenum strategicmove();    //!< \brief Find the strategic move for this position.
    movenum forced();           //!< \brief Find the forced move (if any).
    int winner();               //!< \brief Determine the winner.
    //! \brief Determine if there's a winning sequence of forces.
    int sequence(bool verbose);
    int val(int i) {return points[i].val();}    //!< Who's here?
    void untake(int where);     //!< \brief Revoke a move.
    void take(int where);       //!< \brief Move to a spot.
    void give(int where);       //!< \brief Assume an opponent move to a spot.
    bool canwin() {return status.canwin();} //!< \brief Can 1st player win this turn?
    void highlight() {status.highlight();}  //!< \brief Highlight a winning line
    bool is_forcing(int where); //!< \brief Is the given move forcing?
    /// Is the given move available?
    bool isempty(int where) {
        return points[where].is_empty();
    }
    /// Clear the board.
    void clear(void);

    /// Transform a move from internal (0-63) to external (1-4 104 1-4) form.
    static int external(int m) {
        return (m<0) ? m : ((int(m/16+1)*10+(int(m/4)%4+1))*10 + (m%4+1));
    }

    /// Transform a move from external to internal form
    static int internal(int m) {
        return ((int(m/100)-1)*4+(int(m/10)%10-1))*4 + (m%10-1);
    }

    // Methods for validation
    void mymoveat(int where, char *canonic);
    /// Phase 1 validation output
    void outboard(std::ofstream &strm); //!< \brief Output the board.
    void setstdstring(const char *p, iso** theiso = NULL);  //!< \brief Describe the board.
    void challenge(int i, char *canonic);
    void setposition(char *);
    void setmovelist(int where, int *list, int &count, iso* theiso);
    void outtree(char *,int ,char *, char);     //!< \brief Output a line of the full tree.
};

/**
 * Marks the point empty and reduces the play count.
 * \param where the location to move to, as an index in the array of points.
 */
INLINE void
board::untake(int where) {
    Assert<bad_move>(NASSERT || (plays >= 0 && plays < 64));
    points[where].blank();
    plays--;
}

/**
 * Marks the point taken and increases the play count.
 * \param where the location to move to, as an index in the array of points.
 */
INLINE void
board::take(int where) {
    Assert<bad_move>(NASSERT || (plays >= 0 && plays < 64));
    points[where].take();
    moves[plays++] = where;
}

/**
 * Marks the point taken and increases the play count.
 * \param where the location to move to, as an index in the array of points.
 */
INLINE void
board::give(int where) {
    Assert<bad_move>(NASSERT || (plays >= 0 && plays < 64));
    points[where].give();
    moves[plays++] = where;
}
#endif
