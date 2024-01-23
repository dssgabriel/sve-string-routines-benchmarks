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
    char name_old[32];
    /// Name of the new implementation.
    char name_new[32];
    /// Runtime statistics for the old implementation.
    statistics_t rt_old;
    /// Runtime statistics for the new implementation.
    statistics_t rt_new;
    /// Bandwidth statistics of the old implementation.
    statistics_t bw_old;
    /// Bandwidth statistics of the new implementation.
    statistics_t bw_new;
    /// Runtime speedup of new over old.
    double rt_speedup;
    /// Bandwidth speedup of new over old.
    double bw_speedup;
    /// Buffer size used.
    size_t buf_size;
    /// Number of samples.
    size_t nsamples;
    /// Number of repetitions per samples.
    size_t nreps;
} benchmark_t;

/// Processes the results of a benchmark.
void bench_process(
    benchmark_t self[static 1], size_t nsamples, double old[nsamples], double new[nsamples]
);

/// Prints the results of a benchmark.
void bench_print(benchmark_t const self[static 1]);
