/**
 * @file traverse_bursttrie.hpp
 * @brief header file for traverse_bursttrie.cpp
 * @parblock
 * SortMeRNA - next-generation reads filter for metatranscriptomic or total RNA
 * @copyright 2012-16 Bonsai Bioinformatics Research Group
 * @copyright 2014-16 Knight Lab, Department of Pediatrics, UCSD, La Jolla
 *
 * SortMeRNA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SortMeRNA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with SortMeRNA. If not, see <http://www.gnu.org/licenses/>.
 * @endparblock
 *
 * @contributors Jenya Kopylova, jenya.kopylov@gmail.com
 *               Laurent Noé, laurent.noe@lifl.fr
 *               Pierre Pericard, pierre.pericard@lifl.fr
 *               Daniel McDonald, wasade@gmail.com
 *               Mikaël Salson, mikael.salson@lifl.fr
 *               Hélène Touzet, helene.touzet@lifl.fr
 *               Rob Knight, robknight@ucsd.edu
 */

#pragma once


#include <string>
#include <vector>
#include <iostream> // std::cout
#include <cstring> // std::memcpy
#include <algorithm> // std::copy_n
#include <cstdint> // uint32_t

#include "bitvector.hpp"
#include "options.hpp"


 // Universal Levenshtein table for k=1
extern uint32_t table[4][16][14];

/* 
 * for each 18-mer hit on the read, we store the
 * key to find the positions and the window number
 * on the read at which the 18-mer occurs 
 */
struct id_win
{
	uint32_t id; // key to find index positions
	uint32_t win; // the associated window number on the read 

	id_win() : id(0), win(0) {}
	id_win(uint32_t id, uint32_t win) : id(id), win(win) {}

	id_win(std::string str)
	{
		if (str.size() == sizeof(id) + sizeof(win))
		{
			std::memcpy(static_cast<void*>(&id), str.data(), sizeof(id));
			std::memcpy(static_cast<void*>(&win), str.data()+sizeof(id), sizeof(win));
		}
		else
		{
			ERR("string size ", str.size(), " not equal to ", sizeof(id) + sizeof(win), " Cannot restore.");
			exit(1);
		}
	}

	std::string toString()
	{
		std::string buf;
		std::copy_n(static_cast<char*>(static_cast<void*>(&id)), sizeof(id), std::back_inserter(buf));
		std::copy_n(static_cast<char*>(static_cast<void*>(&win)), sizeof(win), std::back_inserter(buf));
		return buf;
	}
};

/*! @fn traversetrie_align()
	@brief
	@detail Exact matching of [p_1] in [s_1] is completed fully
	in the trie nodes, continue parallel traversal of the trie
	beginning at [s_2]:<br/>

		seed =    |------ [s_1] ------|------ [s_2] ------|<br/>
		pattern = |------ [p_1] ------|------ [p_2] --....--|<br/>
				  |------ trie -------|----- tail ----....--|<br/>

	@param  NodeElement*     trie_t             root node to mini burst trie
	@param  uint32_t         lev_t              initial Levenshtein automaton state
	@param  unsigned char    depth              trie node depth
	@param  MYBITSET*        win_k1_ptr         pointer to start of forward L/2-mer bitvector
	@param  MYBITSET*        win_k1_full        pointer to start of structure storing all bitvectors
	@param  bool&            accept_zero_kmer   if true, if a match is found during forward subsearch, then skip reverse subsearch
	@param  vector<id_win>&  id_hits            vector storing IDs of all candidate L-mers (matching in mini burst trie)
	@param  uint32_t         win_num            sliding window (seed) number on read
	@param  uint32_t         partialwin
	@return void
*/
void traversetrie_align(
	NodeElement * trie_t,
	uint32_t lev_t,
	unsigned char depth,
	UCHAR * win_k1_ptr,
	UCHAR * win_k1_full,
	bool & accept_zero_kmer,
	std::vector<id_win> & id_hits,
	uint32_t win_num,
	uint32_t partialwin,
	Runopts & opts
);