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

#include "stats.h"

#include <math.h>

double mean(size_t n, double const d[n]) {
    double m = 0.0;
    for (size_t i = 0; i < n; ++i) {
        m += d[i];
    }
    return m / (double)n;
}

double stddev(size_t n, double const d[n], double mean) {
    double s = 0.0;
    for (size_t i = 0; i < n; ++i) {
        s += (d[i] - mean) * (d[i] - mean);
    }
    return sqrt(s / (double)(n - 1));
}
