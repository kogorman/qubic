/***************************************************************************
                          llink.h  -  description
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
 * \brief Declaration of class link.
 */

#ifndef LINK_H
#define LINK_H

#include "qval.h"

/// An element (or head) of a doubly-linked list.

//! An llink (list link) may be imbedded in another class to allow linked lists of that type.
//! Its name whould be just "link" but that conflicts with unistd.h.

class llink {
private:
    llink* left;     ///< Pointer to the left.
    llink* right;    ///< Pointer to the right.
    llink* head;     ///< Pointer to the head of the list.
public:
    llink *myhead(){return head; }  ///< The head of the list this cell is in
    llink *next()  {return right; } ///< The next cell on this list.
    llink(){makehead();}            ///< Make this the head of the list.
    /// Add a cell to this list.
    //! Must be called on the head of the list.
    void add(llink *what) {
        Assert<bad_list>(NASSERT || head==this);
        Assert<bad_cell>(NASSERT || what->head != this);
        what->head = this;
        what->left = this;
        (what->right = right)->left = what;
        right = what;
    }
    /// Return a member of this list.
    //! Must be called on the head of the list
    llink *member() {
        Assert<bad_list>(NASSERT || head==this);
        return right;
    }
    /// construct me on a list
    llink(llink& myHead) {
        myHead.add(this);
    }
    /// Make me the head of my own list.
    void makehead() {
        head = left = right = this;
    }
    /// Drop me from the list I'm in.
    void drop() {
        Assert<bad_list>(NASSERT || head!=this);
        llink* oL = left;
        llink* oR = right;
        oL->right = oR;
        oR->left = oL;
        head=NULL;
        left=right=this;
    }
    /// Output me on cerr.
    void linkdump () {
        cerr << "  Head is ";
        if (head != this) cerr << "NOT ";
        cerr << "me." << endl;
    }

    /// Am I an empty list
    bool isempty() {
        Assert<bad_list>(NASSERT || head==this);
        return (right==this && left==this);
    }
};

#endif
