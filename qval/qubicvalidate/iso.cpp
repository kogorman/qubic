/***************************************************************************
                          iso.cpp  -  description
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
 * \brief Member functions of class iso.
 *
 * An iso object represents an isomorphism (mapping) of the board onto an
 * equivalent board.  The obvious ones are rotations and reflections, but
 * there are others.
 */


#include "iso.h"

int iso::nextiso;
iso iso::isos[200];
short iso::mulcache[192][192];

	
void
iso::printall() {
	for (int i=0; i<nextiso; i++) {
		cout << i << isos[i];
	}
}

void
iso::show() {
	int b,l,c,i;
	for (l=0; l<4; l++) {
		cout << endl;
		for (b=0; b<4; b++) {
		  for (c=0; c<4; c++) {
		  	i = val(b,l,c);
		  	cout << setw(2) << i << " ";
		  }
		  cout << "  ";
		}
	}
	cout << endl;
}


ostream& operator<<(ostream&s, iso& what) {
	s << endl;
	for (int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			for(int k=0; k<4; k++) {
				s << setw(3) << what.val(i,j,k);
			}
			if (j<3) {s << "   ";}
		}
		s << endl;
	}
	return s;
}

// Initialize a list of the isomorphisms.
// isos[0] is the identity (initialized by constructor)
// isos[1] is a reflection
// isos[2] is a rotation around the first axis (board)
// isos[3] is a rotation around the second axis (line)
// isos[4] is the "inversion" isomorphism
// isos[5] is the "scramble" isomorphism
// all the others are generated from these.

void
iso::init(void) {
	int i,j,k,ii,jj,kk;
	nextiso = 0;

	for (i=0; i<192; i++) {
		for (j=0; j<192; j++) {
			mulcache[i][j]=-1;
		}
	}

	// A reflection (any one will do)
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			for (k=0; k<4; k++) {
				isos[1].cell(i,j,k) = isos[0].cell(i,j,3-k);
			}
		}
	}

	// A rotation about the i axis
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			for (k=0; k<4; k++) {
				isos[2].cell(i,j,k) = isos[0].cell(i,k,3-j);
			}
		}
	}

	// A rotation about the j axis
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			for (k=0; k<4; k++) {
				isos[3].cell(i,j,k) = isos[0].cell(k,j,3-i);
			}
		}
	}

	// The inversion isomorphism
	for (i=0; i<4; i++) {
		switch(i) {
		case 0:	ii=1; break;
		case 1: ii=0; break;
		case 2: ii=3; break;
		case 3: ii=2; break;
		default: throw "major bummer";
		}
		for (j=0; j<4; j++) {
			switch(j) {
			case 0:	jj=1; break;
			case 1: jj=0; break;
			case 2: jj=3; break;
			case 3: jj=2; break;
			default: throw "major bummer";
			}
			for (k=0; k<4; k++) {
				switch(k) {
				case 0:	kk=1; break;
				case 1: kk=0; break;
				case 2: kk=3; break;
				case 3: kk=2; break;
				default: throw "major bummer";
				}
				isos[4].cell(i,j,k) = isos[0].cell(ii,jj,kk);
			}
		}
	}

	// The scramble isomorphism
	for (i=0; i<4; i++) {
		switch(i) {
		case 0:	ii=0; break;
		case 1: ii=2; break;
		case 2: ii=1; break;
		case 3: ii=3; break;
		default: throw "major bummer";
		}
		for (j=0; j<4; j++) {
			switch(j) {
			case 0:	jj=0; break;
			case 1: jj=2; break;
			case 2: jj=1; break;
			case 3: jj=3; break;
			default: throw "major bummer";
			}
			for (k=0; k<4; k++) {
				switch(k) {
				case 0:	kk=0; break;
				case 1: kk=2; break;
				case 2: kk=1; break;
				case 3: kk=3; break;
				default: throw "major bummer";
				}
				isos[5].cell(i,j,k) = isos[0].cell(ii,jj,kk);
			}
		}
	}

	// Now do all possible compositions
	// This is brute-force, and it takes a few seconds on
	// modern machines (2000), but the code is clearly correct,
	// so I let it stand.

	// We optimize slightly by not using the identity at all, and
	// not composing a reflection with itself.
	
	// We optimize a whole bunch more by noting that we only need
	// the first six generators on the left.  The others will
	// be generated in the first pass.  Unexpected luck.	
	int oldiso = 0;
#ifdef SAYISO
	bool same = false;
#endif

	nextiso = 6;
	while (nextiso != oldiso) {
		oldiso = nextiso;
		for (int i=1; i<6; i++) {
			for (int j=2; j<nextiso; j++) {
				isos[nextiso] = (isos[i] * isos[j]);
				nextiso++;
				if (nextiso>198) {
					cerr << "There are too many (" << nextiso << " isomorphisms" << endl;
					exit(1);
				}
#ifdef SAYISO
				same = true;
#endif
				for (int k=0; k+1<nextiso; k++) {
					if (isos[nextiso-1] == isos[k]) {
						nextiso--;
#ifdef SAYISO
						same = false;
#endif
						break;
					}
				}
#ifdef SAYISO
				if (same) cout << "Iso " << (nextiso-1) << " = compose(" << i << "," << j << ")" << endl;
#endif			
			}
		}
	}
#ifndef NDEBUG
	cout << "There are " << nextiso << " isomorphisms" << endl;
#endif
#ifdef INVERSES
	// Finally, we find the inverses of all the isomorphisms.
	for (int i=0; i<nextiso; i++) {
		// Skip those whose inverse is already known
		if (isos[i]._inverse == (iso *)0) {
	 		bool found=false;
		  for (int j=0; j<nextiso; j++) {
		  	if (isos[j]._inverse == (iso *)0) {
		  		bool okay = true;
		  		for (k=0; k<64; k++) {
		  			if (k != isos[j].index[isos[i].index[k]]){
		  				okay=false;
		  				break;
		  			}
		  		}
		  		if (okay) {
		  			isos[i]._inverse = &isos[j];
		  			isos[j]._inverse = &isos[i];
		  			found = true;
		  			break;
		  		}
		  	}
		  }
		  Assert<bad_init>(NASSERT || found);
		}
	}
#endif
	return;
}
