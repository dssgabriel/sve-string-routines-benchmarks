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

#include "types.h"

/// Statistics of a benchmark.
typedef struct statistics_s {
    /// Minimum recorded time (in ns).
    double min;
    /// Median recorded time (in ns).
    double med;
    /// Maximum recorded time (in ns).
    double max;
    /// Average (mean) recorded time (in ns).
    double avg;
    /// Percentage of error (standard deviation) of recorded time.
    double err;
} statistics_t;

/// Computes the mean of a set of data of size `n`.
double mean(size_t n, double const d[n]);

/// Computes the standard deviation of a set of data of size `n`.
double stddev(size_t n, double const d[n], double mean);
