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

#include "bench.h"
#include "driver.h"
#include "types.h"
#include "utils.h"

#include <assert.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#define NSAMPLES 31ULL

#define HALF_L1D 32768ULL
#define FULL_L1D 65536ULL
#define HALF_L2 524288ULL
#define FULL_L2 1048576ULL
#define HALF_L3 16777216ULL
#define FULL_L3 33554432ULL
#define DRAM 268435456ULL

static inline size_t determine_warmup_cnt(size_t bench_reps) {
    return bench_reps / 10;
}

void bench_memcmp(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t memcmp_bench = {
            .name_old = "Current `memcmp_sve`",
            .name_new = "New `memcmp_sve`",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random char initialization
        char* s1 = malloc(buf_sizes[b] + 1);
        char* s2 = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            s1[c] = (char)(rand() % 128);
            s2[c] = s1[c]; // Make data identical to avoid early function exit
        }
        s1[buf_sizes[b]] = '\0';
        s2[buf_sizes[b]] = '\0';

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            memcmp(s1, s2, buf_sizes[b]);
        }

        // Run benchmark
        driver_memcmp(
            NSAMPLES, bench_reps[b], samples_old, __memcmp_aarch64_sve, s1, s2, buf_sizes[b]
        );
        driver_memcmp(
            NSAMPLES, bench_reps[b], samples_new, new_memcmp_aarch64_sve, s1, s2, buf_sizes[b]
        );

        // Process and display results
        bench_process(&memcmp_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&memcmp_bench);

        // Cleanup
        free(s1);
        free(s2);
    }
}

void bench_strchr(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strchr_bench = {
            .name_old = "Current `strchr_sve`",
            .name_new = "New `strchr_sve`",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* s = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            s[c] = (char)(rand() % 95 + 32);
        }
        s[buf_sizes[b]] = '\0';
        int32_t c = 0; // Look for '\0'

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            strchr(s, c); // Use libc implem for warmup
        }

        // Run benchmark
        driver_strchr(NSAMPLES, bench_reps[b], samples_old, __strchr_aarch64_sve, s, c);
        driver_strchr(NSAMPLES, bench_reps[b], samples_new, new_strchr_aarch64_sve, s, c);

        // Process and display results
        bench_process(&strchr_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strchr_bench);

        // Cleanup
        free(s);
    }
}

void bench_strcmp(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strcmp_bench = {
            .name_old = "Current `strcmp_sve`",
            .name_new = "New `strcmp_sve`",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* s1 = malloc(buf_sizes[b] + 1);
        char* s2 = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            s1[c] = (char)(rand() % 95 + 32);
            s2[c] = s1[c]; // Make strings identical to avoid early function exit
        }
        s1[buf_sizes[b]] = '\0';
        s2[buf_sizes[b]] = '\0';

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            strcmp(s1, s2);
        }

        // Run benchmark
        driver_strcmp(NSAMPLES, bench_reps[b], samples_old, __strcmp_aarch64_sve, s1, s2);
        driver_strcmp(NSAMPLES, bench_reps[b], samples_new, new_strcmp_aarch64_sve, s1, s2);

        // Process and display results
        bench_process(&strcmp_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strcmp_bench);

        // Cleanup
        free(s1);
        free(s2);
    }
}

void bench_strcpy(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strcpy_bench = {
            .name_old = "Current `strcpy_sve`",
            .name_new = "New `strcpy_sve`",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* src = malloc(buf_sizes[b] + 1);
        char* dst = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            src[c] = (char)(rand() % 95 + 32);
        }
        src[buf_sizes[b]] = '\0';
        dst[buf_sizes[b]] = '\0';

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            strcpy(dst, src);
        }

        // Run benchmark
        driver_strcpy(NSAMPLES, bench_reps[b], samples_old, __strcpy_aarch64_sve, dst, src);
        driver_strcpy(NSAMPLES, bench_reps[b], samples_new, new_strcpy_aarch64_sve, dst, src);

        // Process and display results
        bench_process(&strcpy_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strcpy_bench);

        // Cleanup
        free(src);
        free(dst);
    }
}

void bench_strlen(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strlen_bench = {
            .name_old = "Current `strlen_sve`",
            .name_new = "New `strlen_sve`",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* s = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            s[c] = (char)(rand() % 95 + 32);
        }
        s[buf_sizes[b]] = '\0';

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            strlen(s);
        }

        // Run benchmark
        driver_strlen(NSAMPLES, bench_reps[b], samples_old, __strlen_aarch64_sve, s);
        driver_strlen(NSAMPLES, bench_reps[b], samples_new, new_strlen_aarch64_sve, s);

        // Process and display results
        bench_process(&strlen_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strlen_bench);

        // Cleanup
        free(s);
    }
}

void bench_strncmp(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strncmp_bench = {
            .name_old = "Current `strncmp_sve`",
            .name_new = "New `strncmp_sve`",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* s1 = malloc(buf_sizes[b] + 1);
        char* s2 = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            s1[c] = (char)(rand() % 95 + 32);
            s2[c] = s1[c]; // Make strings identical to avoid early function exit
        }
        s1[buf_sizes[b]] = '\0';
        s2[buf_sizes[b]] = '\0';

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            strncmp(s1, s2, buf_sizes[b]);
        }

        // Run benchmark
        driver_strncmp(
            NSAMPLES, bench_reps[b], samples_old, __strncmp_aarch64_sve, s1, s2, buf_sizes[b]
        );
        driver_strncmp(
            NSAMPLES, bench_reps[b], samples_new, new_strncmp_aarch64_sve, s1, s2, buf_sizes[b]
        );

        // Process and display results
        bench_process(&strncmp_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strncmp_bench);

        // Cleanup
        free(s1);
        free(s2);
    }
}

void bench_strnlen(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strnlen_bench = {
            .name_old = "Current `strnlen_sve`",
            .name_new = "New `strnlen_sve`",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* s = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            s[c] = (char)(rand() % 95 + 32);
        }
        s[buf_sizes[b]] = '\0';

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            strnlen(s, buf_sizes[b]);
        }

        // Run benchmark
        driver_strnlen(
            NSAMPLES, bench_reps[b], samples_old, __strnlen_aarch64_sve, s, buf_sizes[b]
        );
        driver_strnlen(
            NSAMPLES, bench_reps[b], samples_new, new_strnlen_aarch64_sve, s, buf_sizes[b]
        );

        // Process and display results
        bench_process(&strnlen_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strnlen_bench);

        // Cleanup
        free(s);
    }
}

void bench_strrchr(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strrchr_bench = {
            .name_old = "Current `strrchr_sve`",
            .name_new = "New `strrchr_sve`",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* s = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            s[c] = (char)(rand() % 95 + 32);
        }
        s[buf_sizes[b]] = '\0';
        int32_t c = 0; // Look for '\0'

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            strrchr(s, c);
        }

        // Run benchmark
        driver_strrchr(NSAMPLES, bench_reps[b], samples_old, __strrchr_aarch64_sve, s, c);
        driver_strrchr(NSAMPLES, bench_reps[b], samples_new, new_strrchr_aarch64_sve, s, c);

        // Process and display results
        bench_process(&strrchr_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strrchr_bench);

        // Cleanup
        free(s);
    }
}

int32_t main(int32_t argc, char* argv[argc + 1]) {
    // Feel free to add additional sizes here
    size_t const buf_sizes[] = { HALF_L1D, FULL_L1D, HALF_L2, FULL_L2, HALF_L3, FULL_L3, DRAM };
    // Don't forget to add appropriate benchmark repetitions here too
    size_t const bench_reps[] = { 10000, 5000, 250, 100, 10, 5, 1 };
    size_t const nbench = sizeof(buf_sizes) / sizeof(buf_sizes[0]);

    assert(
        sizeof(buf_sizes) == sizeof(bench_reps)
        && "Number of buffer sizes and bench repetitions don't match"
    );

    static struct option longopts[] = {
        { "memcmp", no_argument, 0, 'm' },
        { "strchr", no_argument, 0, 's' },
        { "strcmp", no_argument, 0, 'e' },
        { "strcpy", no_argument, 0, 'c' },
        { "strlen", no_argument, 0, 'l' },
        { "strncmp", no_argument, 0, 'p' },
        { "strnlen", no_argument, 0, 'n' },
        { "strrchr", no_argument, 0, 'r' },
        { "help", no_argument, 0, 'h' },
        { "version", no_argument, 0, 'v' },
        { 0, 0, 0, 0 }
    };

    while (true) {
        int32_t optidx = 0;
        int32_t opt = getopt_long(argc, argv, "mseclpnrhv", longopts, &optidx);
        if (opt == -1) { break; }

        switch (opt) {
            case 'm':
                bench_memcmp(nbench, buf_sizes, bench_reps);
                break;
            case 's':
                bench_strchr(nbench, buf_sizes, bench_reps);
                break;
            case 'e':
                bench_strcmp(nbench, buf_sizes, bench_reps);
                break;
            case 'c':
                bench_strcpy(nbench, buf_sizes, bench_reps);
                break;
            case 'l':
                bench_strlen(nbench, buf_sizes, bench_reps);
                break;
            case 'p':
                bench_strncmp(nbench, buf_sizes, bench_reps);
                break;
            case 'n':
                bench_strnlen(nbench, buf_sizes, bench_reps);
                break;
            case 'r':
                bench_strrchr(nbench, buf_sizes, bench_reps);
                break;
            case 'h':
                help();
                exit(0);
            case 'v':
                version();
                exit(0);
            default:
                exit(1);
        }
    }

    return 0;
}
