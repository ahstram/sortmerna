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
 * file: references.cpp
 * created: Dec 23, 2017 Sat
 */
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype> // std::isspace
#include <ios>
#include <cstdint>
#include <locale>

#include "references.hpp"
#include "refstats.hpp"
#include "options.hpp"
#include "common.hpp"


/**
 * load to memory the Reference records from a given index part
 * Read the reference file, extract the part's references into an array (vector) of reference objects
 */
void References::load(uint32_t idx_num, uint32_t idx_part, Runopts & opts, Refstats & refstats)
{
	num = idx_num;
	part = idx_part;
	uint32_t numseq_part = refstats.index_parts_stats_vec[idx_num][idx_part].numseq_part;

	std::ifstream ifs(opts.indexfiles[idx_num].first, std::ios_base::in | std::ios_base::binary); // open reference file

	if (!ifs.is_open())
	{
		ERR("Could not open file ", opts.indexfiles[idx_num].first);
		exit(EXIT_FAILURE);
	}

	// set the file pointer to the first sequence added to the index for this index file section
	ifs.seekg(refstats.index_parts_stats_vec[idx_num][idx_part].start_part);
	if (ifs.fail())
	{
		ERR("Could not locate the reference file ", opts.indexfiles[idx_num].first, " used to construct the index");
		exit(EXIT_FAILURE);
	}

	// load references sequences, skipping the empty lines & spaces
	size_t num_seq_read = 0; // count of sequences in the reference file
	std::string line;
	References::BaseRecord rec;
	bool isFastq = true;
	bool lastRec = false;

	for (int count = 0; num_seq_read != numseq_part; )
	{
		if (!lastRec) std::getline(ifs, line);

		if (line.empty() && !lastRec)
		{
			if (ifs.eof()) lastRec = true;
			continue;
		}

		if (lastRec)
		{
			if (!rec.isEmpty)
			{
				rec.id = rec.getId();
				rec.nid = num_seq_read;
				buffer.push_back(rec);
				num_seq_read++;
			}
			break;
		}

		// remove whitespace (removes '\r' too)
		line.erase(std::find_if(line.rbegin(), line.rend(), [l = std::locale{}](auto ch) { return !std::isspace(ch, l); }).base(), line.end());
		//line.erase(std::remove_if(begin(line), end(line), [l = std::locale{}](auto ch) { return std::isspace(ch, l); }), end(line));
		//if (line[line.size() - 1] == '\r') line.erase(line.size() - 1); // remove trailing '\r'
		// fastq: 0(header), 1(seq), 2(+), 3(quality)
		// fasta: 0(header), 1(seq)
		if (line[0] == FASTA_HEADER_START || line[0] == FASTQ_HEADER_START)
		{
			if (!rec.isEmpty)
			{
				rec.id = rec.getId();
				rec.nid = num_seq_read;
				buffer.push_back(rec); // push record created before this current header
				rec.isEmpty = true;
				num_seq_read++;
				count = 0;
			}

			// start new record
			rec.clear();
			isFastq = (line[0] == FASTQ_HEADER_START);
			rec.format = isFastq ? BIO_FORMAT::FASTQ : BIO_FORMAT::FASTA;
			rec.header = line;
			rec.isEmpty = false;
		} // ~header or last record
		else
		{
			if (isFastq && count > 3) 
			{
				ERR("too many lines (> 4) for FASTQ file");
				exit(EXIT_FAILURE);
			}

			++count; // count the four FASTQ lines

			if (isFastq && line[0] == '+') continue;

			if (isFastq && count == 3)
			{
				rec.quality = line;
				continue;
			}

			convert_fix(line);
			rec.sequence += line;
		} // ~not header
		if (ifs.eof()) lastRec = true; // push and break
	} // ~for
} // ~References::load

  // convert sequence to numerical form and fix ambiguous chars
void References::convert_fix(std::string & seq)
{
	for (std::string::iterator it = seq.begin(); it != seq.end(); ++it)
	{
		if (*it != 32) // space
			*it = nt_table[(int)*it];
	}
}

std::string References::convertChar(int idx)
{
	std::stringstream ss;
	std::string chstr;
	//const char nt_map[5] = { 'A', 'C', 'G', 'T', 'N' }; // TODO: move to common
	for (std::string::iterator it = buffer[idx].sequence.begin(); it != buffer[idx].sequence.end(); ++it)
	{
		if (*it < 5)
			chstr += nt_map[(int)*it];
		else
		{
			ss << "ERROR: string is not in numeric format. Encountered character: " << *it << std::endl;
			std::cerr << ss.str();
			exit(EXIT_FAILURE);
		}
	}
	return chstr;
}

int References::findref(const std::string& id)
{
	int retpos = -1;
	for (unsigned i = 0; i < buffer.size(); ++i)
	{
		if (std::string::npos != buffer[i].header.find(id)) {
			retpos = i;
			break; 
		}
	}

	return retpos;
}

void References::unload()
{
	buffer.clear(); // TODO: is this enough?
} // ~References::clear
