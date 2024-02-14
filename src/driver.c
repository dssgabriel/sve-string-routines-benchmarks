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

#define _GNU_SOURCE

#include "driver.h"
#include "utils.h"

#include <time.h>

/// Utility macro defining the body of a driver function that benchmarks a given routine.
#define DRIVER_BODY(fn, ...)                                                                       \
    struct timespec a, b;                                                                          \
    for (size_t e = 0; e < nsamples; ++e) {                                                        \
        clock_gettime(CLOCK_MONOTONIC_RAW, &a);                                                    \
        for (size_t i = 0; i < nreps; ++i) {                                                       \
            fn(__VA_ARGS__);                                                                       \
        }                                                                                          \
        clock_gettime(CLOCK_MONOTONIC_RAW, &b);                                                    \
        samples[e] = elapsed_ns(a, b) / (double)nreps;                                             \
    }

void driver_memcmp(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    memcmp_fn_t* memcmp_fn,
    void const* s1,
    void const* s2,
    size_t n
) {
    DRIVER_BODY(memcmp_fn, s1, s2, n);
}

void driver_memcpy(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    memcpy_fn_t* memcpy_fn,
    void* restrict dst,
    void const* restrict src,
    size_t n
) {
    DRIVER_BODY(memcpy_fn, dst, src, n);
}

void driver_strcmp(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strcmp_fn_t* strcmp_fn,
    char const* s1,
    char const* s2
) {
    DRIVER_BODY(strcmp_fn, s1, s2);
}

void driver_strncmp(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strncmp_fn_t* strncmp_fn,
    char const* s1,
    char const* s2,
    size_t n
) {
    DRIVER_BODY(strncmp_fn, s1, s2, n);
}

void driver_strchr(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strchr_fn_t* strchr_fn,
    char const* s,
    int32_t c
) {
    DRIVER_BODY(strchr_fn, s, c);
}

void driver_strrchr(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strrchr_fn_t* strrchr_fn,
    char const* s,
    int32_t c
) {
    DRIVER_BODY(strrchr_fn, s, c);
}

void driver_strcpy(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strcpy_fn_t* strcpy_fn,
    char* dst,
    char const* src
) {
    DRIVER_BODY(strcpy_fn, dst, src);
}

void driver_strncpy(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strncpy_fn_t* strncpy_fn,
    char* dst,
    char const* src,
    size_t n
) {
    DRIVER_BODY(strncpy_fn, dst, src, n);
}

void driver_strlen(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strlen_fn_t* strlen_fn,
    char const* s
) {
    DRIVER_BODY(strlen_fn, s);
}

void driver_strnlen(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strnlen_fn_t* strnlen_fn,
    char const* s,
    size_t n
) {
    DRIVER_BODY(strnlen_fn, s, n);
}
