/*
 @copyright 2016-2021  Clarity Genomics BVBA
 @copyright 2012-2016  Bonsai Bioinformatics Research Group
 @copyright 2014-2016  Knight Lab, Department of Pediatrics, UCSD, La Jolla

 @parblock
 SortMeRNA - next-generation reads filter for metatranscriptomic or total RNA
 This is a free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SortMeRNA is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with SortMeRNA. If not, see <http://www.gnu.org/licenses/>.
 @endparblock

 @contributors Jenya Kopylova   jenya.kopylov@gmail.com
			   Laurent No�      laurent.noe@lifl.fr
			   Pierre Pericard  pierre.pericard@lifl.fr
			   Daniel McDonald  wasade@gmail.com
			   Mika�l Salson    mikael.salson@lifl.fr
			   H�l�ne Touzet    helene.touzet@lifl.fr
			   Rob Knight       robknight@ucsd.edu
*/

/*
 * @file bitvector.hpp
 * @brief Function and variable definitions for bitvector.cpp
 */

 /** @file bitvector.hpp */

#ifndef BITVECTOR_H
#define BITVECTOR_H

/* for map matrix */
#include "indexdb.hpp"

/* a bitvector of 1 byte */
#define UCHAR unsigned char

/* mask for depth > 0 bitvectors for k = 1 */
#define MSB4 (UCHAR)4

/* mask for depth = 0 bitvectors for k = 1 */
#define MSB8 (UCHAR)8	


/* initialized in paralleltraversal.cpp */
extern int _win_num;
extern int thiswindow;
extern int _readn;
extern int thisread;




/*
 *
 * FUNCTION 	: void init_win_k1   ( char*, MYBITSET*, MYBITSET* )
 * PURPOSE	: create the first bitvector table of the 17-mer window on the read at position i = 0.
 * PARAMETERS	: 	  
 *
 **************************************************************************************************************/
void init_win_f ( char*, UCHAR*, UCHAR*, int numbvs );
void init_win_r ( char*, UCHAR*, UCHAR*, int numbvs );


/*
 *
 * FUNCTION 	: void offset_win_k1 ( char*, char*, MYBITSET*, MYBITSET*, MYBITSET* )
 * PURPOSE	: for each 17-mer window after the first one on the read, the bitvector table is
 *		  computed from the previous one by means of bitshifting and looking only at the new character
 *		  of the read
 * PARAMETERS	: 
 *		  	  
 *
 **************************************************************************************************************/
void offset_win_k1 ( char*, char*, UCHAR*, UCHAR*, UCHAR*, int numbvs );


/*
 *
 * FUNCTION 	: void output_win_k1 ( MYBITSET*, bool )
 * PURPOSE	: output the bitvector table (for debugging)
 * PARAMETERS	: 	  
 *
 **************************************************************************************************************/
void output_win_k1 (UCHAR*, bool, int partialwin );

#endif 
