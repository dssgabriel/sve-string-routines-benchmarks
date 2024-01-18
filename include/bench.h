/**
 * Copyright © 2004 - 2024, Université de Versailles Saint-Quentin-en-Yvelines (UVSQ)
 * Copyright © 2024, Gabriel Dos Santos
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 **/

#pragma once

#include "stats.h"
#include "types.h"

/// Benchmark information.
typedef struct benchmark_s {
    /// Name of the old implementation.
    char old_name[20];
    /// Name of the new implementation.
    char new_name[20];
    /// Statistics for the old implementation.
    statistics_t old_stats;
    /// Statistics for the new implementation.
    statistics_t new_stats;
    /// Speedup of new over old.
    double speedup;
    /// Buffer size used.
    size_t buf_size;
    /// Number of samples.
    size_t nsamples;
    /// Number of iterations (repetitions) per samples
    size_t niters;
} benchmark_t;

/// Processes the results of a benchmark.
void bench_process(
    benchmark_t self[static 1], size_t nsamples, double old[nsamples], double new[nsamples]
);

/// Prints the results of a benchmark.
void bench_print(benchmark_t self[static 1]);

