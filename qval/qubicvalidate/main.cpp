/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Sat Apr 28 16:37:20 PDT 2001
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
 * \brief Main function and associated static thingies.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "qval.h"
#include "iso.h"
#include "win.h"
#include "point.h"
#include "line.h"
#include "board.h"
#include "strategic.h"

/// output to phase1.out
/*
 * Gets positions where opponent is not forced.  These become the inputs
 * to the rerun until all such positions have been generated.
 */
std::ofstream phaseout;
/// output to checkstrategic.txt
std::ofstream checkstrategic;
/// ouput to base_strategic.out
/**
 * These are the strategic moves, copied when examining the base position.
 * The file is used for comparison by the validation script.
 */
std::ofstream basestrategic;
/// output to reachedstrategic.out.
std::ofstream reachedstrategic;
/// output to "treefile", which is based on "tree.out" or just "tree."
/**
 * Phase 1: <census> <start pos> <move> <nextpos> s (3068 lines)
 *                strategic moves and their isomorphs
 *
 * Phase 2: <census> <start pos> <move> <nextpos> d (unforced player 2) (see main())
 *          <census> <start pos> <move> <nextpos> r (first player forced) (see challenge())
 *          <census> <start pos> <move> <nextpos> f (opponent was forced) (see mymoveat())
 */
std::ofstream tree;
/// output to "phase2.in"
std::ifstream prior;
/// input from "checkfile", which is "checkstrategic.uniq" or based on  "check.*"
/**
 * Only the check.* files are used in the current regime.
 */
std::ifstream readstrategic;

/// Global verbosity flag.
bool verbose;

void
readmove(board *b) {
    int m,i;
    m=-1;
    while (m<0) {
        cin >> m;
        i = board::internal(m);
        if (i<0 || i>63 || cin.fail() || !b->isempty(i)) {
            cout << "Enter a 3-digit number, for an empty cell." << endl;
            m = -1;
            cin.clear();
            continue;
        }
        b->give(i);
        b->show();
    }
}


//************************************************************************** makemove(board *)
// makemove(board *b) -- the heart of the matter
//    Choose a move, in this order:
//        1. If there's a win, take it straightaway
//        2. Otherwise, if the opponent is forcing, reply as required
//        3. Otherwise, look for a "strategic" move, if any.
//        4. Otherwise, find a forcing sequence and begin it.
// The work of Oren Patashnik showed that this procedure works,
// and guarantees a win.  The tricky bits are finding the
// "strategic" moves, which Oren did and which I copied, and
// finding the forcing sequence in a resonable time.  As of
// version 0.1, the sequence finding is pretty much brute-force,
// and I rely on a fast processor to make it workable.
int
makemove(board *b) {
    int m,r;
    r=0;
    m = b->winner();
    if (m<0) {
        m = b->forced();
        if (m>=0 && verbose) {
            cout << "Force me?  All right, for one turn...." << endl;
        }
    } else {
        if (verbose && b->forced() >= 0) {
            cout << "Force me?  You're too late!" << endl;
        }
        r = 1;    // there's a winner
    }
    if (m < 0) {
        m = b->strategicmove();
    }
    if (m < 0) {
      m = b->sequence(verbose);
    }
    if (m<0) {
        cout << "Can find no move" << endl;
        b->show();
        b->showcanonic();
    }
    Assert<bad_result>(NASSERT || m>=0);
    cout << "My move is " << board::external(m) << endl;
    b->take(m);
    if (r) b->highlight();
    b->show();
    return r;
}

//************************************************************************** usage(char *)
static void
usage(char *me) {
    cout << "usage: " << me << " [-v] [-V] [phasenumber] [-s[suffix]]" << endl;
    cout << "       -v: verbose: Qubic brags about how well it's doing" << endl;
    cout << "       -V: version: print the version number and exit" << endl;
}

//************************************************************************** main(int, char **)
/**
 * The usual thing.  Usage:
 *     qval [-v] [-V] [phasenumber] [-s[suffix]]
 *
 * Get it started, run through the steps, quit.
 */
int main(int argc, char *argv[])
{
    int argn;
    verbose = false;
    bool sayVersion = false;
    long int phase = -1;
    char *endptr;
    char checkfile[20],treefile[20];

    strncpy(checkfile,"checkstrategic.uniq",20);
    strncpy(treefile,"tree.out",20);

#if 0    
#define ANSWERLEN 100
    char answer[ANSWERLEN];
#endif

    for (argn=1; argn<argc; argn++) {
        if (*argv[argn] != '-') {
            // non-switch arg
            if (phase >= 0) {
                usage(argv[0]);
                exit(1);
            } else {
                phase = strtol(argv[argn], &endptr, 10);
                if (*endptr) {
                    // There should be no non-converted chars
                    cerr << "Unconverted chars" << endl;
                    usage(argv[0]);
                    exit(1);
                }
                continue;
            }
        }
        switch (argv[argn][1]) {
        case 'v': verbose = true;
                        break;
        case 'V': sayVersion = true;
                        break;
        case 's':
                    if ((phase != 3)) {
                        cerr << "Bad -s switch" << endl;
                        usage(argv[0]);
                        exit(1);
                    }
                    strcpy(checkfile,"check.");
                    strcpy(treefile,"tree.");
                    strncat(checkfile,&argv[argn][2],3);
                    strncat(treefile,&argv[argn][2],3);
                    break;
                
        default:
                    cerr << "Unknown argument: " << argv[argn] << endl;
                    usage(argv[0]);
                    exit(1);
        }
    }
    
    if (verbose) {
        cout << "treefile is " << treefile << endl;
        cout << "checkfile is " << checkfile << endl;
    }

    try {
        char inputline[65];
        char startcanonic[65];
        char resultcanonic[65];
        int movelist[64];
        int movecount;

        int checked = 0, dots = 0;
        int i, st, move;
        iso::init();                // Build the isomorphisms of the Qubic board
        win::init();                // Find the winning lines
        strategic::init();          // Prepare 2929 strategic moves

        board b;                    // must come after initializations
        
        // Phase 1: take all strategic moves, but do not follow forcing chains.
        //                    The result is the opponent's move, and
        //                    the position is output for processing in phase 2,
        //                    in all isomorphic variations.
        // Phase 2: take all possible opponent moves to inputs, plus forced 1st player
        //                    moves.  The result is one of
        //                    1: 1st player unforced move.
        //                    2: the opponent's move (after at least one forced reply)
        //                      in which case this will have to be considered some more.
        // Phase 3: check the accumulated positions which should be strategic, they
        //                    actually should be either strategic or forced wins.    
    
        switch(phase) {
        case 1:
            // Phase 1 Outputs
            // to base_strategic.out: the raw strategic position
            // to tree.out (and tree.??): <census> <start pos> <move> <nextpos> s (3068 lines)
            checkstrategic.open("checkstrategic.txt", ios::out);
            phaseout.open("phase1.out");
            basestrategic.open("base_strategic.out", ios::out);
            tree.open("tree.out", ios::out);
            for (st=0; st<2929; st++) {
                // Set up the board from the strategic object
                strategic &strat = strategic::find(st);
                b.clear();
                for (i=0; i<64; i++) {
                    switch (strat.points[i]) {
                    case point::X:
                        b.take(i);
                        break;
                    case point::O:
                        b.give(i);
                        break;
                    }
                }
                b.setstdstring(startcanonic);
                // Show the starting position
                b.outboard(basestrategic);
                // Compute the appearance of the board after the move (but restore it)
                // Outputs to phaseout (phase1.out) for boards that end on an unforced
                // opponent's move.
                // Outputs to checkstrategic (checkstrategic.txt) for the ones that
                // end on an unforced move by player 1.  These are all positions to be
                // checked for being strategic, which they had better be because they
                // are arrived at by forced moves based on a single strategic move.
                b.mymoveat(strat.moveto, resultcanonic);
                // Compute all the moves that are indistinguishable from the given one.
                // XXX: later may have to accomodate multiple distinguishable strategic
                // moves.
                b.setmovelist(strat.moveto, movelist, movecount, &iso::isos[0]);
                // Output all paths that get us there.  This is probably overkill; one
                // would surely do?
                for (i=0; i<movecount; i++) {
                    b.outtree(startcanonic, movelist[i], resultcanonic, 's');
                }
            }
            phaseout.close();
            checkstrategic.close();
            basestrategic.close();
            tree.close();
            break;
        case 2:
            /*
             * Here we are treating outputs from phase1: positions of unforced player 2
             * moves.
             * If the opponent is forced (see board::canwin()), give the move.
             * If the opponent is free, take all possible moves.
             */
            checkstrategic.open("checkstrategic.txt",ios::app);
            phaseout.open("phase2.out", ios::out);
            prior.open("phase2.in",ios::in);
            tree.open("tree.out", ios::app);

            while (prior.getline(inputline,65)) {
                b.setposition(inputline);
                b.setstdstring(startcanonic);
                if (b.canwin()) {
                    b.challenge(b.winner(),resultcanonic);
                } else {
                    for (i=0; i<64; i++) {
                        if (b.val(i) == point::EMPTY) {
                            b.challenge(i,resultcanonic);
                            b.outtree(startcanonic,i,resultcanonic,'d');
                        }
                    }
                }
            }
            prior.close();
            phaseout.close();
            checkstrategic.close();
            tree.close();
            break;
            
        case 3:
            cout << endl << "Checking " << checkfile << " into " << treefile << endl;
            readstrategic.open(checkfile, ios::in);
            reachedstrategic.open("reachedstrategic.out", ios::app);
            tree.open(treefile, ios::app);
            while (readstrategic.getline(inputline,65)) {
                ++checked;
                cout << "+" ;
                if ((++dots % 100) == 0) { cout << " " << checked << endl; }
                cout.flush();
                tree << endl << inputline << ' ' << checked << endl;
                b.setposition(inputline);
                b.setstdstring(startcanonic);
                if (b.forced() >=0) {
                    cerr << endl << "Forced position included in checkstrategic: " << inputline
                        << endl;
                    continue;
                }
                // Only need search if there's no strategic move
                if (b.strategicmove() == -1 ) {
                    // no strategic move: find forcing sequence and report the sequence.
                    move = b.sequence(verbose);
                    if (move == -1) {
                        cerr << endl << "No forced sequence for " << inputline << " ("
                            << startcanonic << ")" << endl;
                    }
                } else {
                    // It appears this is strategic.  Say we got here.
                    b.outboard(reachedstrategic);
                }
            }
            cerr << endl << checked << " checked!" << endl;
            readstrategic.close();
            reachedstrategic.close();
            tree.close();
            break;

        default:
            Assert<bad_arg>(0);
            break;
        }
    } catch(bad_arg &e) {
        cerr << endl << "BAD_ARG EXCEPTION NOT CAUGHT" << endl;
        throw;
    } catch(bad_class &e) {
        cerr << endl << "BAD_CLASS EXCEPTION NOT CAUGHT" << endl;
        throw;
    } catch(bad_value &e) {
        cerr << endl << "BAD_VALUE EXCEPTION NOT CAUGHT" << endl;
        throw;
    } catch(bad_move &e) {
        cerr << endl << "BAD_MOVE EXCEPTION NOT CAUGHT" << endl;
        throw;
    } catch(bad_init &e) {
        cerr << endl << "BAD_INIT EXCEPTION NOT CAUGHT" << endl;
        throw;
    } catch(bad_cleanup &e) {
        cerr << endl << "BAD_CLEANUP EXCEPTION NOT CAUGHT" << endl;
        throw;
    } catch(bad_list &e) {
        cerr << endl << "BAD_LIST in main()" << endl;
        throw;
    } catch(bad_cell &e) {
        cerr << endl << "BAD_CELL in main()" << endl;
        throw;
    } catch(bad_result &e) {
        cerr << endl << "BAD_RESULT EXCEPTION NOT CAUGHT" << endl;
        throw;
    } catch(std::exception &e) {
        cerr << endl << "STANDARD LIBRARY EXCEPTION NOT CAUGHT" << endl;
        throw;
    } catch(...) {
        cerr << endl << "NON-STANDARD EXCEPTION NOT CAUGHT" << endl;
        throw;
    }
    
    
  return EXIT_SUCCESS;
}
