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

/**
 * file: ssw.hpp
 * created: Oct 28, 2017 Sat
 */

#pragma once

#include <stdint.h>
#include <vector>
#include <iterator>

typedef struct s_align2 {
	std::vector<uint32_t> cigar;
	uint32_t ref_num; // position of the sequence in the reference file [0...number of sequences in the ref.file - 1]
	int32_t ref_begin1;
	int32_t ref_end1;
	int32_t	read_begin1;
	int32_t read_end1;
	uint32_t readlen;
	uint16_t score1; // <- 'ssw.c::ssw_align'
	uint16_t part;
	uint16_t index_num;
	bool strand; // flags whether this alignment was done on a forward (true) or reverse-complement (false) read.

	// default construct
	s_align2() : ref_num(0), ref_begin1(0), ref_end1(0), read_begin1(0), read_end1(0), readlen(0), score1(0), part(0), index_num(0), strand(false) {}

	// construct from binary string
	s_align2(std::string bstr)
	{
		size_t offset = 0;
		size_t len = 0; // cigar, refname length

		// cannot use copy/copy_n - VC error C4996: 'std::copy_n::_Unchecked_iterators
		std::memcpy(static_cast<void*>(&len), bstr.data(), sizeof(len));
		offset += sizeof(len);

		cigar.assign(len, 0);
		std::memcpy(static_cast<void*>(cigar.data()), bstr.data() + offset, len * sizeof(uint32_t));
		offset += len * sizeof(uint32_t);
		// ref_num
		std::memcpy(static_cast<void*>(&ref_num), bstr.data() + offset, sizeof(ref_num));
		offset += sizeof(ref_num);
		// ref_begin1
		std::memcpy(static_cast<void*>(&ref_begin1), bstr.data() + offset, sizeof(ref_begin1));
		offset += sizeof(ref_begin1);
		// ref_end1
		std::memcpy(static_cast<void*>(&ref_end1), bstr.data() + offset, sizeof(ref_end1));
		offset += sizeof(ref_end1);
		// read_begin1
		std::memcpy(static_cast<void*>(&read_begin1), bstr.data() + offset, sizeof(read_begin1));
		offset += sizeof(read_begin1);
		// read_end1
		std::memcpy(static_cast<void*>(&read_end1), bstr.data() + offset, sizeof(read_end1));
		offset += sizeof(read_end1);
		// readlen
		std::memcpy(static_cast<void*>(&readlen), bstr.data() + offset, sizeof(readlen));
		offset += sizeof(readlen);
		// score1
		std::memcpy(static_cast<void*>(&score1), bstr.data() + offset, sizeof(score1));
		offset += sizeof(score1);
		// part
		std::memcpy(static_cast<void*>(&part), bstr.data() + offset, sizeof(part));
		offset += sizeof(part);
		// index_num
		std::memcpy(static_cast<void*>(&index_num), bstr.data() + offset, sizeof(index_num));
		offset += sizeof(index_num);
		// strand
		std::memcpy(static_cast<void*>(&strand), bstr.data() + offset, sizeof(strand));
		offset += sizeof(strand);
	}

	// convert to binary string
	std::string toString()
	{
		std::string buf;
		// length of cigar
		size_t cigarlen = cigar.size();
		char* beginIt = static_cast<char*>(static_cast<void*>(&cigarlen));
		std::copy_n(beginIt, sizeof(cigarlen), std::back_inserter(buf));
		// cigar
		beginIt = static_cast<char*>(static_cast<void*>(cigar.data()));
		std::copy_n(beginIt, cigarlen * sizeof(cigar[0]), std::back_inserter(buf));
		
		beginIt = static_cast<char*>(static_cast<void*>(&ref_num));
		std::copy_n(beginIt, sizeof(ref_num), std::back_inserter(buf)); // ref_num
		beginIt = static_cast<char*>(static_cast<void*>(&ref_begin1));
		std::copy_n(beginIt, sizeof(ref_begin1), std::back_inserter(buf)); // ref_begin1
		beginIt = static_cast<char*>(static_cast<void*>(&ref_end1));
		std::copy_n(beginIt, sizeof(ref_end1), std::back_inserter(buf)); // ref_end1
		beginIt = static_cast<char*>(static_cast<void*>(&read_begin1));
		std::copy_n(beginIt, sizeof(read_begin1), std::back_inserter(buf)); // read_begin1
		beginIt = static_cast<char*>(static_cast<void*>(&read_end1));
		std::copy_n(beginIt, sizeof(read_end1), std::back_inserter(buf)); // read_end1
		beginIt = static_cast<char*>(static_cast<void*>(&readlen));
		std::copy_n(beginIt, sizeof(readlen), std::back_inserter(buf)); // readlen
		beginIt = static_cast<char*>(static_cast<void*>(&score1));
		std::copy_n(beginIt, sizeof(score1), std::back_inserter(buf)); // score1
		beginIt = static_cast<char*>(static_cast<void*>(&part));
		std::copy_n(beginIt, sizeof(part), std::back_inserter(buf)); // part
		beginIt = static_cast<char*>(static_cast<void*>(&index_num));
		std::copy_n(beginIt, sizeof(index_num), std::back_inserter(buf)); // index_num
		// strand
		beginIt = static_cast<char*>(static_cast<void*>(&strand));
		std::copy_n(beginIt, sizeof(strand), std::back_inserter(buf));

		return buf;
	} // ~toString

	// for serialization
	size_t size() {
		return sizeof(uint32_t) * cigar.size()
			+ sizeof(ref_num)
			+ sizeof(ref_begin1)
			+ sizeof(ref_end1)
			+ sizeof(read_begin1)
			+ sizeof(read_end1)
			+ sizeof(readlen)
			+ sizeof(score1)
			+ sizeof(part)
			+ sizeof(index_num)
			+ sizeof(strand);
	}

	bool operator==(const s_align2& other)
	{
		return other.cigar == cigar &&
			other.ref_num == ref_num &&
			other.ref_begin1 == ref_begin1 &&
			other.ref_end1 == ref_end1 &&
			other.read_begin1 == read_begin1 &&
			other.read_end1 == read_end1 &&
			other.readlen == readlen &&
			other.score1 == score1 &&
			other.part == part &&
			other.index_num == index_num &&
			other.strand == strand;
	}
} s_align2;