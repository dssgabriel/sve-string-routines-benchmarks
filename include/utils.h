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

#include <time.h>

#define xstr(s) str(s)
#define str(s) #s

/// Compute elapsed time in nanoseconds between two `struct timespec` points in time.
double elapsed_ns(struct timespec a, struct timespec b);

/// Converts nanoseconds (as `double`) to seconds.
double ns_to_s(double ns);

/// Compares two `double` values (used in `qsort`).
int32_t cmp_double(void const* a, void const* b);

/// Random buffer initialization helper.
void init_buf_rand(size_t n, char** buf, bool is_str);

/// Buffer copy helper.
void init_buf_copy(size_t n, char** buf_dst, char const* buf_src);

/// Prints program help.
void help(void);

/// Prints program version.
void version(void);
