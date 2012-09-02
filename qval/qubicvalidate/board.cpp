/***************************************************************************
                          board.cpp  -  description
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
 * \brief Member functions of the board class.
 *
 * Only those member functions which are not inline-able are here; some are
 * conditional, depending on whether the NINLINE manifest constant is defined.
 * The inlineable ones are perforce in the header file.
 */


#include "board.h"
#include "point.h"
#include "line.h"

//****************************************************************** init()
/**
 * Ensure a good start.  That means all points are empty, and the array of lines
 * (winnable sets of 4 points) are linked to the proper points.  That sets both
 * the content and the topology of the game arena.
 */
void
board::init() {
    seqlevel = 0;
    seqboards = 0;
    plays = 0;
    forcing = 0;
    // points are auto-initialized (I hope)
    for (int i=0; i<64; i++) {
        Assert<bad_init>(NASSERT || points[i].is_empty());
        points[i].me = i;
    }
    try {
        // Link up each line to this board and all its points
        for (int i=0; i<76; i++) {
            lines[i].linkup(i,points,this);
        }
    } catch(bad_list &e) {
        cerr << endl << "BAD_LIST in board::init()" << endl;
        throw;
    } catch(...) {
        cerr << endl << "Assertion failure in board::init()" << endl;
        throw;
    }
};

//****************************************************************** clear()
/**
 * Clear the board completely.
 */
void
board::clear() {
    int i;
    seqlevel = 0;
    seqboards = 0;
    for (i=0; i<64; i++) {
        if (!isempty(i)) untake(i);
    }
    plays = 0;
}


//****************************************************************** show(iso&)
/**
 * Show the board under a specific isomorphism.
 * \param v The isomorphism to use.
 */
void
board::show(const iso &v) {
    int i;

    for (int l=0; l<4; l++) {
        cout << endl;
#ifndef NDEBUG
        cout << setw(seqlevel*2) << "" ;
#endif
        for (int b=0; b<4; b++) {
          for (int c=0; c<4; c++) {
              i = v.val(b,l,c);
              switch(points[i].val()) {
              case point::EMPTY:
                  cout << "- "; break;
              case point::X:
                  cout << "x "; break;
              case point::O:
                  cout << "o "; break;
              case point::BIGX:
                  cout << "X "; break;
              case point::BIGO:
                cout << "O "; break;
              default:
                  Assert<bad_value>(NASSERT);
              }
          }
          cout << "  ";
        }
    }
    cout << endl;
}

//****************************************************************** canonical()
/**
 * Returns a pointer to an isomorphism under which this board is in canonical
 * form.  This may not be unique, in which case a random choice is made.
 * \return A pointer to an isomorphism under which this board is in canonical form.
 */
iso *
board::canonical() {
    int i, s, candidate[198], it;
    candidate[0] = s = 0;
    i=1;
    for (int j=1; j<iso::nextiso; j++) {
        int result=0;
        for (int k=0; k<64; k++) {
            if (points[iso::isos[s].index[k]].val()
                    > points[iso::isos[j].index[k]].val()) {
                // the one we're considering is "less" than the current winner
                result = 1;
                break;
            } else if(points[iso::isos[s].index[k]].val()
                    < points[iso::isos[j].index[k]].val()) {
                // The one we're considering is "greater" than the current winner
                // -- a new winner
                result = -1;
                break;
            }
        }
        if (result<0) {
            // a new winner
            candidate[0] = s = j;
            i = 1;
        } else if (result == 0) {
            // a tie with the current winner
            candidate[i++] = j;
        }
    }
#ifndef NDEBUG
    cout << "There are " << i << " canonical forms" << endl;
#endif
    if (i>1) {
        it = random() % i;
#ifndef NDEBUG
        cout << "Picking number " << it << endl;
#endif
    } else {
        it = 0;
    }
#ifndef NDEBUG
    cout << "Canonical form:" << endl;
    show(iso::isos[candidate[it]]);
#endif
    return &iso::isos[candidate[it]];
}

//********************************************************** strategicmove()
/**
 * Determines if there is a strategic move for the current position (if any).
 * \return a strategic move if there is one; otherwise returns -1.
 */
int
board::strategicmove() {
    int i;
    iso *stdForm = canonical();

    for (i=0; i<strategic::i; i++) {
        bool okay=true;
        for (int k=0; k<64; k++) {
            if (strategic::smv[i].points[k] != points[stdForm->index[k]].val()) {
                okay = false;
                break;
            }
        }
        if (okay) {
            int res =stdForm->index[strategic::smv[i].moveto];
#ifndef NDEBUG
            cout << "Strategic move (canonic) for " << strategic::smv[i].pattern
                    << " is " << strategic::smv[i].moveto << endl;
            cout << "Stragegic move (board) is " << res << endl;
            cout << "Using this iso:" << endl;
            stdForm->show();
#endif
            return res;
        }
    }
#ifndef NDEBUG
    cout << "Must be a forced win somewhere" << endl;
#endif
    return -1;
}

//****************************************************************** winner()
/**
 * Determines if there is a winning move.
 * \return a winning move if there is one, otherwise -1.
 */
int
board::winner() {
    line *theWin;
    
    if (!status.canwin()) return -1;
    theWin = status.theWin();
    return theWin->openPoint();
}

//****************************************************************** forced()
/**
 * Determines if a reply is forced.
 * \return a move that is a forced reply, if any, otherwise -1.
 */
int
board::forced() {
    line *theForce;
    
    if (!status.haveforced()) return -1;
    theForce = status.theForce();
    return theForce->openPoint();
}

//********************************************************** sequence(bool)
/**
 * This is the non-recursive head function.
 * Finds a sequence of forcing moves to a direct win, or to another strategic
 * move. This is the publicly visible version of this; it calls the private
 * one, which does the recursion.
 * \param verbose whether to output bragging messages.
 * \return a move that begins a winning forced sequence if any, otherwise -1.
 */
int
board::sequence(bool verbose) {
    bound bnd;
    int rem;
    
    forcing = plays;
    seqlevel = 0;                    // initialize debugging stuff
    seqboards = 0;
    haveSolution = false;
    
    // Try first with a small bound.  That is, try for a quick win.
    bnd = sequence(plays+12);
    // If that fails, try next with a modest bound.  That is, try harder.
    if (bnd.where<0 && plays + 12 < 65) {
        bnd=sequence(plays+24);
    }
    // If that still doesn't work, try real hard by allowing any winning sequence.
    if (bnd.where<0 && plays + 24 < 65) {
        bnd=sequence(65);
    }
    if (bnd.where>=0 && verbose) {
        rem = (bnd.depth-plays)/2;
        if (rem <2) {
            cout << "I will win next turn" << endl;
        } else {
            cout << "I will win in " << rem << " plays" << endl;
            cout << "I looked at " << seqboards << " positions to find this move."
                << endl;
        }
    }
    forcing = 0;
    return bnd.where;
}

//********************************************************** sequence(int)
/** 
 * \overload
 * This is the private recursive tail to the public function.
 * Finds a sequence of forcing moves to a direct win, or to another strategic
 * move, under a limit on the sequence length.   This method uses both
 * trimming and pruning.  Pruning is used when a winning sequence has been
 * found: no search is allowed to look for a longer sequence.  Trimming is
 * used directly on any winning sequence, and removes plays which do not
 * contribute to the win. (These are plays whose removal along with the
 * opponent's forced answer leaves the resulting subsequence still a winner).
 * When trimming removes one or more plays from the sequence, this current
 * level will be deeper than the new pruning bound.  This is reported as a
 * failure, but with a shorter bound.  Generally, this will cause quick exit
 * from over-deep search levels.
 * \param blim a bound limit.
 * \return a bound object representing a move and a length.
 */
board::bound
board::sequence(int blim) {
    int targets[64], winners[64], scores[64];
    int i,m,f,w;
    int  forces;
    bound bnd,res;
    int currdepth = blim;

    char mycanonic[65];
    iso* myiso;
    char resultcanonic[65];
    iso* resultiso;
    
    w=0;
    bnd.where = -1;
    bnd.depth = currdepth;
#ifndef QUBICVALIDATE
    if ((++seqboards) % 50000 == 0) {cout << "."; cout.flush();};
#endif
    seqlevel++;
#ifndef NDEBUG
    if (haveSolution && (seqboards > 50000)) return bnd;
    cout << setw(seqlevel*2) << "" << "Considering my move on this board:" << endl;
    show();
#endif
    if (plays >= currdepth) {
#ifndef NDEBUG
        cout << setw(seqlevel*2) << "" << "Leaving because of depth bound " << endl;
#endif
        seqlevel--;
        return bnd;
    }
        
    // can I win outright?
    m = winner();
    if (m>=0) {
#ifndef NDEBUG
        cout << setw(seqlevel*2) << "" << "I can win outright at "
            << external(m) << endl;
#endif
        bnd.where = m;

#ifdef NOTRIM
        bnd.depth = plays;
#else
        bnd.depth = trim();
#endif

    } else {
        // If not, am I forced?
        setstdstring(mycanonic, &myiso);
        m = forced();
        bnd.where = -1;
        if (m>=0) {
            // The only possibility is if the forced move is itself forcing
            take(m);
            f = forced();    // Am I still forced?  If so, I just lost.
            if (f<0) {
                if (status.canwin()) {    // is it forcing?
                    res = willwin(currdepth);        // is it winning too?
                    bnd.depth = res.depth;            // adjust (short winner?)
                    if (res.where>=0) {
#ifndef NDEBUG
                        cout << setw(seqlevel*2) << ""
                            << "I am forced, but my reply is winning. " << endl;
#endif
                        bnd.where = res.where;
                        setstdstring(resultcanonic, &resultiso);
                        outtree(mycanonic, myiso->inverse()->val(res.where), resultcanonic, 'c');
                    }
#ifndef NDEBUG
                        else {
                            cout << setw(seqlevel*2) << ""
                    << "My forced reply is forcing, but does not win.  Sigh." << endl;
                    }
#endif
                }
#ifndef NDEBUG
                    else {
                        cout << setw(seqlevel*2) << ""
                            << "I would be forced here, ending the sequence " << endl;
                }
#endif
            }
#ifndef NDEBUG
                else {
                    cout << setw(seqlevel*2) << "" << "I would lose here, to " << external(f)
                        << ", " << external(m) << endl;
            }
#endif
            untake(m);
        } else {
            // Try all unforced forcing moves.  Take the best winner, if any.
            // (Best means shortest forcing sequence.)

            // Step 1: find the forcing moves.
            forces = status.canForceAt(targets);
#ifndef NDEBUG
            cout << "I can force at:";
            for (i=0; i<forces; i++) {
                cout << " " << external(targets[i]);
            }
            cout << endl;
#endif

            // Step 2: score the moves
            for (i=0; i<forces; i++) {
                scores[i] = points[targets[i]].score();
            }

            // Step 3: take each in turn, in order by score.  This is done by selection
            //   sort, incrementally at the loop top.
            for (i=0; i<forces; i++) {
                int max = scores[i];
                int kmax = i;
                int k;
                for (k=i+1; k<forces; k++) {
                    if (scores[k]>max) {kmax=k; max = scores[kmax];}
                }
                if (kmax != i) {
                    k = targets[i];  targets[i] = targets[kmax]; targets[kmax] = k;
                    k = scores[i];   scores[i]  = scores[kmax];  scores[kmax] = k;
                }

                // now try the winner.
                take(targets[i]);
#ifndef NDEBUG
                    cout << setw(seqlevel*2) << "" << "Considering the move to "
                        << external(targets[i]) << endl;
#endif
                res = willwin(currdepth);
                if (res.depth < currdepth) {
                    currdepth = res.depth;
                    w = 0;
                }
                if (res.where >= 0 && plays<currdepth) {
#ifndef NDEBUG
                    cout << setw(seqlevel*2) << "" << "The move to " << external(targets[i])
                        << " wins after " << res.depth << " moves" << endl;
#endif
                    winners[w++] = targets[i];
                }
                untake(targets[i]);

#ifdef QUBICVALIDATE
                // For validation, one winner is enough
                if (w) break;
#endif

                if (plays >= currdepth) {
#ifndef NDEBUG
                    cout << setw(seqlevel*2) << "" << "Rewinding because depth bound has changed"
                        << endl;
#else
                    if (verbose) {
                        cout << "Finding a really short winning sequence..." << endl;
                    }
#endif
                    seqlevel--;
                    bnd.where = -1;
                    bnd.depth = currdepth;
                    return bnd;
                }
            }

            // Now sort through and record the winning moves.
            if (w) {
                for (i=0; i<w; i++) {
                    take(winners[i]);
                    setstdstring(resultcanonic, &resultiso);
                    outtree(mycanonic, myiso->inverse()->val(winners[i]), resultcanonic, 'c');
                    untake(winners[i]);
                }
                if (w>1) {
#ifndef NDEBUG
                    cout << setw(seqlevel*2) << "" << "Picking from among " << w
                        << " winning moves" << endl;
#endif
                    bnd.where = winners[random()%w];
                } else {
#ifndef NDEBUG
                    cout << setw(seqlevel*2) << "" << "Just one winner among these" << endl;
#endif
                    bnd.where = winners[0];
                }
                bnd.depth = currdepth;
#ifndef NDEBUG
                    cout << setw(seqlevel*2) << "" << "Reporting success with a move to "
                        << external(bnd.where) << " at depth " << currdepth << endl;
#endif                
            }
        }
    }
    seqlevel--;
#ifndef NDEBUG
    cout << setw(seqlevel*2) << "" << "Reporting move " << external(bnd.where)
            << ", at depth " << bnd.depth << " after looking at " << seqboards << " boards"
            << endl;
#endif
    if (bnd.where >= 0) haveSolution = true;
    return bnd;
}

//****************************************************************** willwin()
/**
 * Assesses whether I can force a win after the opponent moves.  Requires a
 * force (by me against opponent) to be in effect.
 * \param blim the limit on the sequence length.
 * \return a bound on the reply.
 */
board::bound
board::willwin(int blim) {
    int m;
    bound bnd;
    char mycanonic[65];
    iso *myiso;
    char resultcanonic[65];
    iso *resultiso;
    
    // There must be a force in effect.
    Assert<bad_arg>(NASSERT || status.canwin());
    setstdstring(mycanonic, &myiso);
    m = winner();            // where opponent must block
    give(m);
    bnd = sequence(blim);        // can I still win?
    if (bnd.where>=0) {
        setstdstring(resultcanonic, &resultiso);
        outtree(mycanonic, myiso->inverse()->val(m), resultcanonic, 'b');
    }
    untake(m);
    return bnd;
}

//****************************************************************** trim()
/**
 * Trims the current sequence, starting at the first forcing move. Moves not
 * required for the force are removed, starting at the end.  The resulting
 * pruning depth is reported.
 * \return the pruning depth.
 */
int
board::trim() {
    movenum trimmed[64], original[64];
    bool okay;
    int i,j,k,m;
    int oplays = plays;
    int trimlim = plays;
    
#ifndef NDEBUG
    cout << "Trimming from " << forcing << " to " << plays << endl;
#endif
    
    for (i=0; i<plays; i++) {
        original[i] = trimmed[i] = moves[i];
    }

    for (i=plays-2; i>=forcing; i -= 2) {
#ifndef NDEBUG
        cout << "Untake " << external(moves[i+1]) << " and " << external(moves[i]) << endl;
#endif
        untake(original[i+1]);
        untake(original[i]);
    
        // having 'untaken' some moves, see if skipping the current one leaves the
        // forcing sequence a winner.  If so, we 'trim' this move and its answer from the
        // list.
        okay = true;
        for (j=i+2; j<trimlim; j+=2) {
            if (is_forcing(trimmed[j])) {
#ifndef NDEBUG
                cout << "Take(" << external(trimmed[j]) << ")" << endl;
#endif                
                take(trimmed[j]);
                m = winner();                        // where opponent must block
                if (m==trimmed[j+1]) {    // still on track
#ifndef NDEBUG
                    cout << "Give(" << external(m) << ")" << endl;
#endif                
                    give(m);
                } else {
#ifndef NDEBUG
                    cout << "Now the force is to " << external(m) << " instead of "
                        << external(trimmed[j+1]) << "; restoring " << external(original[i])
                        << endl;
#endif                
#ifndef NDEBUG
                cout << "Untake(" << external(trimmed[j]) << ")" << endl;
#endif                
                    untake(trimmed[j]);
                    okay = false;
                    break;
                }
            } else {
#ifndef NDEBUG
                cout << "Now " << external(trimmed[j]) << " is no longer forcing; have to leave "
                    << external(original[i]) << " in the sequence" << endl;
#endif                
                okay = false;
                break;
            }
        }

        if (!canwin()) {
#ifndef NDEBUG
            if (okay) cout << "Now the end of the sequence is no longer a win; have to leave "
                << external(original[i]) << " in the sequence" << endl;
#endif                
            okay = false;
        }

#ifndef NDEBUG
            cout << "Undoing testing, from " << i+2 << " to " << trimlim << endl;
#endif                

        // undo the results of testing
        for (k=i+2; k<j; k+=2) {
#ifndef NDEBUG
            cout << "Untake(" << external(trimmed[k]) << ")" << endl;
#endif                
            untake(trimmed[k]);
#ifndef NDEBUG
            cout << "Untake(" << external(trimmed[k+1]) << ")" << endl;
#endif                
            untake(trimmed[k+1]);
        }
        // if it's still okay, trim out this move and response
        if (okay) {
#ifndef NDEBUG
            cout << "Trimming out " << external(original[i]) << " and " << external(original[i+1])
                << endl;
#endif                
            for (j=i+2; j<trimlim; j+=2) {
                trimmed[j-2] = trimmed[j];
            }
            trimlim -= 2;
        }
    }
    
    for (i=forcing; i<oplays; i+=2) {
#ifndef NDEBUG
        cout << "Take " << external(moves[i]) << "; give " << external(moves[i+1]) << endl;
#endif
        take(original[i]);
        give(original[i+1]);
    }
    Assert<bad_cleanup>(NASSERT || plays==oplays);
    return oplays;
}

//********************************************************** is_forcing(int)
/**
 * Test if a particular move is forcing.
 * \param where the potential move.
 * \return true if forcing, false if not.
 */
bool
board::is_forcing(int where) {
    take(where);
    bool res=canwin();
    untake(where);
    return res;
}

// Begin things for validation only

//********************************************************** mymoveat(int)
/**
 * Validate a move at \a where.  Used in Phase 1 of validation on the
 * indicated strategic moves, and in Phase 2 on unforced possible moves.
 */
void
board::mymoveat(int where, char *argcanonic) {
    char resultcanonic[65];
    int his, i;
    int movelist[64];
    int movecount;
    iso *myiso;

    // take the indicated move.
    take(where);
    setstdstring(argcanonic, &myiso);
    Assert<bad_value>(NASSERT || (forced() == -1) );
    // now test if the opponent is forced, and follow where that leads.
    // This can work out two ways:
    //   1. when it's my turn again, I am not forced.  Surely this must be
    //      one of the strategic positions.  XXX: the logic here is not
    //      at all clear to me now.  Why could it not be just another step
    //      where I have all the forcing moves, and the opponent does not?
    //   2. When it's my turn again, I am forced.  Take the forced move,
    //      and recurse.
    // On the other hand, if the opponent is not forced, this position goes
    // out as a phase 1 output.  This can happen after a number of forcing
    // moves, or directly.
    if (canwin()) {
        // opponent is forced.
        his = winner();         // where he is forced to
        challenge(his, resultcanonic);            // recurse: take the move and see what happens
        setmovelist(his, movelist, movecount, myiso);
        for (i=0; i<movecount; i++) {
            outtree(argcanonic,movelist[i],resultcanonic,'f');
        }
    } else {
        // unforced opponent.  Output this position to phase output.
        outboard(phaseout);
    }    
    untake(where);
}        

//********************************************************** challenge(int)
/**
 * Phase 2: the opponent is moving to \a where.
 * If the move is forcing, we have to enter mymoveat(); otherwise,
 * it will have to be checked for being strategic or forceable.
 * \param where where to assume an opponent's move.
 * \param argcanonic (output) pointer to a char buffer for recording results.
 */
void
board::challenge(int where, char *argcanonic) {
    int m, i;
    char resultcanonic[65];
    int movelist[64];
    int movecount;
    iso *myiso;

    give(where);
    setstdstring(argcanonic, &myiso);
    // now it's my move.  Is it forced?
    m = forced();
    if (m >= 0) {
        mymoveat(m, resultcanonic);  // recurse
        setmovelist(m, movelist, movecount, myiso);
        for (i=0; i<movecount; i++) {
            outtree(argcanonic, movelist[i], resultcanonic, 'r');
        }
    } else {                                                                          
        // I am not forced.  Might be strategic, or might
        // have further forcing moves.  Put on the queue of things
        // to check.
        /* Comment 1 Sept 06: only 6 lines of output.  All strategic,
         * so these are just the forcing sequences of length 1.
         */
        outboard(checkstrategic);
    }
    untake(where);
}

//************************************************** outboard(std::ofstream)
/**
 * Output the board to the given stream.
 * \param strm a std::ofstream to send the results to.
 */
void
board::outboard(std::ofstream &strm) {
    char buf[65];
    setstdstring(buf);
    strm << buf << endl;
}

//****************************************** setstdstring(char *, iso **)
/**
 * Places the standard description of the current board into the buffer
 * supplied. NOTE: this will be the canonic description, so it is likely to
 * have been transformed.    
 * \param p the supplied buffer
 * \param theIso the isomorphism to use (optional)
 */
void
board::setstdstring(const char *p, iso **theIso) {
    iso &view = *canonical();
    if (theIso) *theIso = &view;
    int i;
    int blanks=0;
    char *rp = (char *)p;

    for (i=0; i<64; i++) {
        switch(points[view.index[i]].val()) {
        case point::X:
        case point::BIGX:
            if (blanks) {
                if (blanks>9) {
                    *rp++ = '0' + int(blanks/10);
                }
                *rp++ = '0' + (blanks%10);
            }
            *rp++ = 'x';
            blanks = 0;
            break;
        case point::O:
        case point::BIGO:
            if (blanks) {
                if (blanks>9) {
                    *rp++ = '0' + int(blanks/10);
                }
                *rp++ = '0' + (blanks%10);
            }
            *rp++ = 'o';
            blanks = 0;
            break;
        case point::EMPTY:
            blanks++;
            break;
        default:
            Assert<bad_value>(NASSERT);
        }
    }
    *rp = '\0';
}

//********************************************************** setposition(char *)
/**
 * Set the board position according to a description string.  See strategic.cpp
 * for details.
 * \param desc the description string.
 */
void
board::setposition(char *desc) {
    char *cursor;
    char ch;
    int skip, where;

    clear();

    cursor = desc;
    skip = 0;
    where = 0;
    while ((ch = *cursor)) {
        switch(ch) {
        case 'x': where += skip;
                skip = 0;
                take(where++);
                break;
        case 'o': where += skip;
                skip = 0;
                give(where++);
                break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
                skip *= 10;
                skip += (ch - '0');
                break;
        default:
                Assert<bad_arg>(NASSERT);
        }
        cursor++;
    }
}

//****************************************** setmovelist(int, int[], int)
/**
 * Set the movelist "list" to be all equivalent moves to the given one.  This is
 * normally just one move, but can be more when the board is fairly empty or
 * otherwise symmetrical.
 * \param where the original move.
 * \param list (output) where to put the 'clone' moves.
 * \param count (output) count of the clone moves.
 * \param canonicIso the canonic isomorphism
 */
void
board::setmovelist(int where, int *list, int &count, iso *canonicIso) {
    int i, j;
    int isoMove, xMove;
    bool matches;
    iso *theIso;
    isoMove = canonicIso->inverse()->val(where);
    if (canonicIso->index[where] != where) {
        matches=false;
    }
    list[0] = isoMove;
    count = 1;

    for (i=0; i<192; i++) {    // for every view of the board
        theIso = &iso::isos[i];
        xMove = theIso->val(isoMove);
        matches = true;
        for (j=0; j<64; j++) {
            if (points[j].val() != points[theIso->val(j)].val()) {
                matches = false;
                break;
            }
        }    
        if (matches) {                // if the view is an isomorphism of this board
            matches = false;
            // check that this version of the move is not listed already
            for (j=0; j<count; j++) {
                if (list[j] == xMove) {
                    matches=true;
                    break;
                }
            }
            if (!matches) {
                list[count++] = xMove;
            }
        }
    }
}
//****************************************** outtree(char *, int, char *)
/**
 * Output a line of a tree view to the global stream 'tree'.
 * \param from starting position.
 * \param move the move madA.e
 * \param result what you get.
 * \param kind type of move.
 */
void
board::outtree(char *from,int move,char *result, char kind) {
    tree << plays << ' ' << from << ' ' << move << ' ' << result << ' ' << kind << endl;
}
