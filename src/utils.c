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

#include "utils.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BIN_NAME "bench-sve-string-routines"
#define VERSION_MAJOR 0
#define VERSION_MINOR 5
#define VERSION_PATCH 1

inline double elapsed_ns(struct timespec a, struct timespec b) {
    return (double)(b.tv_sec - a.tv_sec) * 1.0e+9 + (double)(b.tv_nsec - a.tv_nsec);
}

inline double ns_to_s(double ns) {
    return ns * 1.0e-9;
}

int32_t cmp_double(void const* a, void const* b) {
    if (*(double const*)a > *(double const*)b) {
        return 1;
    } else if (*(double const*)a < *(double const*)b) {
        return -1;
    } else {
        return 0; 
    }
}

// Assumes buffer has allocated size `n + 1`
inline void init_buf_rand(size_t n, char* buf, bool is_str) {
    assert(buf != NULL && "buffer cannot be nullptr");
    if (is_str) {
        for (size_t i = 0; i < n; ++i) {
            // Random alphanumeric character
            buf[i] = (char)(rand() % 95 + 32);
        }
    } else {
        for (size_t i = 0; i < n; ++i) {
            // Random non-null ASCII character
            buf[i] = (char)(rand() % 126 + 1);
        }
    }
    buf[n] = '\0';
}

// Assumes both buffers have the same length `n` (and allocated size `n + 1`)
inline void init_buf_copy(size_t n, char* buf_dst, char const* buf_src) {
    assert(buf_dst != NULL && "destination buffer cannot be nullptr");
    assert(buf_src != NULL && "source buffer cannot be nullptr");
    for (size_t i = 0; i < n; ++i) {
        buf_dst[i] = buf_src[i];
    }
    buf_dst[n] = '\0';
}

void help(void) {
    fprintf(stderr, "Comparative benchmarks for implementations of Arm SVE optimized string routines\n");
    fprintf(stderr, "Copyright (C) 2024, Laboratoire LI-PaRAD, UVSQ\n\n");
    fprintf(stderr, "USAGE:\n\t%s [OPTIONS] [FLAGS]\n", BIN_NAME);
    fprintf(stderr, "\nOPTIONS:\n");
    fprintf(stderr, "\t-x, --memcpy   Runs benchmark for the `memcpy` routine\n");
    fprintf(stderr, "\t-c, --strcpy   Runs benchmark for the `strcpy` routine\n");
    fprintf(stderr, "\t-y, --strncpy  Runs benchmark for the `strncpy` routine\n");
    fprintf(stderr, "\t-m, --memcmp   Runs benchmark for the `memcmp` routine\n");
    fprintf(stderr, "\t-e, --strcmp   Runs benchmark for the `strcmp` routine\n");
    fprintf(stderr, "\t-p, --strncmp  Runs benchmark for the `strncmp` routine\n");
    fprintf(stderr, "\t-s, --strchr   Runs benchmark for the `strchr` routine\n");
    fprintf(stderr, "\t-r, --strrchr  Runs benchmark for the `strrchr` routine\n");
    fprintf(stderr, "\t-l, --strlen   Runs benchmark for the `strlen` routine\n");
    fprintf(stderr, "\t-n, --strnlen  Runs benchmark for the `strnlen` routine\n");
    fprintf(stderr, "\nFLAGS:\n");
    fprintf(stderr, "\t-h, --help     Prints this help and exits\n");
    fprintf(stderr, "\t-v, --version  Prints version and exits\n");
}

void version(void) {
    fprintf(stderr, "%s v%d.%d.%d\n", BIN_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    fprintf(stderr, "Copyright (C) 2024, Laboratoire LI-PaRAD, UVSQ\n");
}
