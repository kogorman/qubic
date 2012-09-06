/***************************************************************************
                          qval.h  -  description
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
 * \brief Top-level file for qubic validation.
 *
 * Contains globals and other common things.
 */

/*! \file runparts.sh
 * Finish the validation by operating on the check files.
 */

/*! \file runtests.sh
 * Run the first few phases of the validation.
 */

#ifndef QVAL_H
#define QVAL_H

#define QUBICVALIDATE
#define NOTRIM

#ifdef QUBICVALIDATE
#define INVERSES
#endif

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <exception>

using namespace std;    // for ease of use

class iso;
extern iso* canonicIso;

/// \brief Bad Argument Exception.
class bad_arg {};
/// \brief Bad List Exception.
class bad_list {};
/// \brief Bad Cell Exception.
class bad_cell {};
/// \brief Bad Value Exception.
class bad_value {};
/// \brief Bad Object Exception.
class bad_object {};
/// \brief Bad Class Exception.
class bad_class {};
/// \brief Bad Result Exception.
class bad_result {};
/// \brief Bad Move Exception.
class bad_move {};
/// \brief Bad Initialization Exception.
class bad_init {};
/// \brief Bad Cleanup Exception.
class bad_cleanup {};

template<class E, class A>inline void Assert(A assertion) {
    if (!assertion) throw E();
};

//! Specify whether to output debugging messages
#define NDEBUG
//! Specify whether to declare suitable functions as "inline".
#define INLINE inline
#undef SAYISO

#ifdef QUBICVALIDATE
const bool NASSERT = false;
#else
#ifdef NDEBUG
const bool NASSERT = true;
#else
const bool NASSERT = false;
#endif
#endif

extern long int qubicRandom();
extern bool verbose;
typedef int movenum;

// strictly for the validation
extern std::ofstream phaseout;
extern std::ofstream checkstrategic;
extern std::ofstream basestrategic;
extern std::ofstream reachedstrategic;
extern std::ofstream tree;

extern std::ifstream prior;
extern std::ifstream readstrategic;

#endif
