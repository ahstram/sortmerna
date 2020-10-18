#pragma once
/**
 * FILE: readstats.hpp
 * Created: Nov 06, 2017 Mon
 *
 * Collective Statistics for all Reads. Encapsulates old 'compute_read_stats' logic and results
 * Some statistics computed during Alignment, and some in Post-processing
 */

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>

#include "common.hpp"
#include "options.hpp"

// forward
class KeyValueDatabase;

/*
 * 1. 'all_reads_count' - Should be known before processing and index loading. 
 * 2. 'total_mapped_sw_id_cov'
 *        Calculated during alignment and stored to KVDB (see paralleltraversal.cpp:align)
 *        Thread accessed from 'compute_lis_alignment' - Synchronize
 * 3. 'reads_matched_per_db' - Synchronize.
 *			Calculated in 'compute_lis_alignment' during alignment. Thread accessed.
 * 4. 'total_reads_denovo_clustering'
 *			TODO: currently accessed in single thread ('computeStats') but potentially could be multiple threads
 *          Setter: 'computeStats'.
 *			User: 'writeLog'
 * 5. 'otu_map' - Clustering of reads around references by similarity i.e. {ref: [read,read,...], ref: [read,read...], ...}
 *			calculated after alignment is done on all reads
 *			Setter: 'computeStats' post-processing callback
 *			User: 'printOtuMap'
 *			TODO: Store in DB? Can be very big.
 */
struct Readstats 
{
	std::string dbkey; // Hashed concatenation of underscore separated basenames of the read files. Used as the key into the Key-value DB. 
	std::string suffix; // 'fasta' | 'fastq' TODO: remove?

	std::atomic<uint32_t> min_read_len; // length of the shortest Read in the Reads file. 'parallelTraversalJob'
	std::atomic<uint32_t> max_read_len; // length of the longest Read in the Reads file. 'parallelTraversalJob'
	std::atomic<uint64_t> total_reads_aligned; // total number of reads passing E-value threshold. Set in 'compute_lis_alignment'
	std::atomic<uint64_t> total_mapped_sw_id_cov; // [2] total number of reads passing E-value, %id, %query coverage thresholds
	std::atomic<uint64_t> short_reads_num; // reads shorter than a threshold of N nucleotides. Reset for each index.

	uint64_t all_reads_count; // [1] total number of reads in file.
	uint64_t all_reads_len; // total number of nucleotides in all reads i.e. sum of length of All read sequences
	uint64_t total_reads_denovo_clustering; // [4] total number of reads for de novo clustering. 'computeStats' post-processing callback

	std::vector<uint64_t> reads_matched_per_db; // [3] total number of reads matched for each database. `compute_lis_alignment`.
	std::map<std::string, std::vector<std::string>> otu_map; // [5] Populated in 'computeStats' post-processor callback

	bool is_stats_calc; // flags 'computeStats' was called. Set in 'postProcess'
	bool is_total_mapped_sw_id_cov; // flag 'total_mapped_sw_id_cov' was calculated (so no need to calculate no more)

	Readstats(uint64_t all_reads_count, uint64_t all_reads_len, KeyValueDatabase& kvdb, Runopts& opts);

	void calcSuffix(Runopts& opts);
	std::string toBstring();
	std::string toString();
	bool restoreFromDb(KeyValueDatabase & kvdb);
	void store_to_db(KeyValueDatabase & kvdb);
	void pushOtuMap(std::string & ref_seq_str, std::string & read_seq_str);
	void printOtuMap(std::string otumapfile);
	void set_is_total_mapped_sw_id_cov();
}; // ~struct Readstats
