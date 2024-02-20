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

#ifdef __ARM_FEATURE_SVE

#define _GNU_SOURCE

#include "bench.h"
#include "driver.h"
#include "types.h"
#include "utils.h"

#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Align memory allocations if specified
#if defined(ALIGNED_ALLOCS)
    #define alloc(size) aligned_alloc(64, size)
#else
    #define alloc(size) malloc(size)
#endif

#define NSAMPLES 101ULL

/// Max string length in bytes for `SMALL_STR`
#define MAX_LEN_IN_BYTES 128ULL

// Define `SMALL_STR` if nothing defined by the user
#if !defined(SMALL_STR) && !defined(FULL_SIZE_RANGE)
#define SMALL_STR
#endif

static inline size_t determine_warmup_cnt(size_t bench_reps) {
    return bench_reps > 10 ? bench_reps / 10 : 1;
}

void bench_memcmp(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t memcmp_bench = {
#ifdef CMP_LIBC
            .name_old = "memcmp (GNU libc 2.39)",
#else
            .name_old = "memcmp (Arm OR 23.01)",
#endif
            .name_new = "memcmp (LI-PaRAD)",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random memory initialization
        char* s1 = alloc(buf_sizes[b] + 1);
        char* s2 = alloc(buf_sizes[b] + 1);
        init_buf_rand(buf_sizes[b], s1, false);
        init_buf_copy(buf_sizes[b], s2, s1); // Make data identical to avoid early function exit

#ifdef DEBUG
        fprintf(stderr, "Checking `new_memcmp_aarch64_sve`... ");
        assert(
            memcmp(s1, s2, buf_sizes[b]) == new_memcmp_aarch64_sve(s1, s2, buf_sizes[b]) &&
            "`new_memcmp_aarch64_sve` failed"
        );
        fprintf(stderr, "OK\n");
#endif

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            (void)memcmp(s1, s2, buf_sizes[b]);
        }

        // Run benchmark
#ifdef CMP_LIBC
        driver_memcmp(NSAMPLES, bench_reps[b], samples_old, memcmp, s1, s2, buf_sizes[b]);
#else
        driver_memcmp(NSAMPLES, bench_reps[b], samples_old, __memcmp_aarch64_sve, s1, s2, buf_sizes[b]);
#endif
        driver_memcmp(NSAMPLES, bench_reps[b], samples_new, new_memcmp_aarch64_sve, s1, s2, buf_sizes[b]);

        // Process and display results
        bench_process(&memcmp_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&memcmp_bench);

        // Cleanup
        free(s1);
        free(s2);
    }
}

void bench_memcpy(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t memcpy_bench = {
#ifdef CMP_LIBC
            .name_old = "memcpy (GNU libc 2.39)",
#else
            .name_old = "memcpy (Arm OR 23.01)",
#endif
            .name_new = "memcpy (LI-PaRAD)",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random char initialization
        char* src = alloc(buf_sizes[b] + 1);
        char* dst = alloc(buf_sizes[b] + 1);
        init_buf_rand(buf_sizes[b], src, false);

#ifdef DEBUG
        fprintf(stderr, "Checking `new_memcpy_aarch64_sve`... ");
        new_memcpy_aarch64_sve(dst, src, buf_sizes[b]);
        assert(memcmp(src, dst, buf_sizes[b]) == 0 && "`new_memcpy_aarch64_sve` failed");
        fprintf(stderr, "OK\n");
#endif

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            (void)memcpy(dst, src, buf_sizes[b]);
        }

        // Run benchmark
#ifdef CMP_LIBC
        driver_memcpy(NSAMPLES, bench_reps[b], samples_old, memcpy, dst, src, buf_sizes[b]);
#else
        driver_memcpy(NSAMPLES, bench_reps[b], samples_old, __memcpy_aarch64_sve, dst, src, buf_sizes[b]);
#endif
        driver_memcpy(NSAMPLES, bench_reps[b], samples_new, new_memcpy_aarch64_sve, dst, src, buf_sizes[b]);

        // Process and display results
        bench_process(&memcpy_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&memcpy_bench);

        // Cleanup
        free(src);
        free(dst);
    }
}

void bench_strcmp(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = {0};
    double samples_new[NSAMPLES] = {0};

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strcmp_bench = {
#ifdef CMP_LIBC
            .name_old = "strcmp (GNU libc 2.39)",
#else
            .name_old = "strcmp (Arm OR 23.01)",
#endif
            .name_new = "strcmp (LI-PaRAD)",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* s1 = alloc(buf_sizes[b] + 1);
        char* s2 = alloc(buf_sizes[b] + 1);
        init_buf_rand(buf_sizes[b], s1, true);
        init_buf_copy(buf_sizes[b], s2, s1);

#ifdef DEBUG
        fprintf(stderr, "Checking `new_strcmp_aarch64_sve`\n");
        assert(
            strcmp(s1, s2) == new_strcmp_aarch64_sve(s1, s2) &&
            "`new_strcmp_aarch64_sve` failed"
        );
#endif

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            (void)strcmp(s1, s2);
        }

        // Run benchmark
#ifdef CMP_LIBC
        driver_strcmp(NSAMPLES, bench_reps[b], samples_old, strcmp, s1, s2);
#else
        driver_strcmp(NSAMPLES, bench_reps[b], samples_old, __strcmp_aarch64_sve, s1, s2);
#endif
        driver_strcmp(NSAMPLES, bench_reps[b], samples_new, new_strcmp_aarch64_sve, s1, s2);

        // Process and display results
        bench_process(&strcmp_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strcmp_bench);

        // Cleanup
        free(s1);
        free(s2);
    }
}

void bench_strncmp(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = {0};
    double samples_new[NSAMPLES] = {0};

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strncmp_bench = {
#ifdef CMP_LIBC
            .name_old = "strncmp (GNU libc 2.39)",
#else
            .name_old = "strncmp (Arm OR 23.01)",
#endif
            .name_new = "strncmp (LI-PaRAD)",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* s1 = alloc(buf_sizes[b] + 1);
        char* s2 = alloc(buf_sizes[b] + 1);
        init_buf_rand(buf_sizes[b], s1, true);
        init_buf_copy(buf_sizes[b], s2, s1);

#ifdef DEBUG
        fprintf(stderr, "Checking `new_strncmp_aarch64_sve`\n");
        assert(
            strncmp(s1, s2, buf_sizes[b]) == new_strncmp_aarch64_sve(s1, s2, buf_sizes[b]) &&
            "`new_strncmp_aarch64_sve` failed"
        );
#endif

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            (void)strncmp(s1, s2, buf_sizes[b]);
        }

        // Run benchmark
#ifdef CMP_LIBC
        driver_strncmp(NSAMPLES, bench_reps[b], samples_old, strncmp, s1, s2, buf_sizes[b]);
#else
        driver_strncmp(NSAMPLES, bench_reps[b], samples_old, __strncmp_aarch64_sve, s1, s2, buf_sizes[b]);
#endif
        driver_strncmp(NSAMPLES, bench_reps[b], samples_new, new_strncmp_aarch64_sve, s1, s2, buf_sizes[b]);

        // Process and display results
        bench_process(&strncmp_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strncmp_bench);

        // Cleanup
        free(s1);
        free(s2);
    }
}

void bench_strchr(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = {0};
    double samples_new[NSAMPLES] = {0};

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strchr_bench = {
#ifdef CMP_LIBC
            .name_old = "strchr (GNU libc 2.39)",
#else
            .name_old = "strchr (Arm OR 23.01)",
#endif
            .name_new = "strchr (LI-PaRAD)",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* s = alloc(buf_sizes[b] + 1);
        init_buf_rand(buf_sizes[b], s, true);
        int32_t c = 0; // Look for '\0'

#ifdef DEBUG
        fprintf(stderr, "Checking `new_strchr_aarch64_sve`\n");
        assert(strchr(s, c) == new_strchr_aarch64_sve(s, c) && "`new_strchr_aarch64_sve` failed");
#endif

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            (void)strchr(s, c);
        }

// Run benchmark
#ifdef CMP_LIBC
        driver_strchr(NSAMPLES, bench_reps[b], samples_old, strchr, s, c);
#else
        driver_strchr(NSAMPLES, bench_reps[b], samples_old, __strchr_aarch64_sve, s, c);
#endif
        driver_strchr(NSAMPLES, bench_reps[b], samples_new, new_strchr_aarch64_sve, s, c);

        // Process and display results
        bench_process(&strchr_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strchr_bench);

        // Cleanup
        free(s);
    }
}

void bench_strrchr(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = {0};
    double samples_new[NSAMPLES] = {0};

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strrchr_bench = {
#ifdef CMP_LIBC
            .name_old = "strrchr (GNU libc 2.39)",
#else
            .name_old = "strrchr (Arm OR 23.01)",
#endif
            .name_new = "strrchr (LI-PaRAD)",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* s = alloc(buf_sizes[b] + 1);
        init_buf_rand(buf_sizes[b], s, true);
        int32_t c = 0; // Look for '\0'

#ifdef DEBUG
        fprintf(stderr, "Checking `new_strrchr_aarch64_sve`\n");
        assert(
            strrchr(s, c) == new_strrchr_aarch64_sve(s, c) &&
            "`new_strrchr_aarch64_sve` failed"
        );
#endif

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            (void)strrchr(s, c);
        }

        // Run benchmark
#ifdef CMP_LIBC
        driver_strrchr(NSAMPLES, bench_reps[b], samples_old, strrchr, s, c);
#else
        driver_strrchr(NSAMPLES, bench_reps[b], samples_old, __strrchr_aarch64_sve, s, c);
#endif
        driver_strrchr(NSAMPLES, bench_reps[b], samples_new, new_strrchr_aarch64_sve, s, c);

        // Process and display results
        bench_process(&strrchr_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strrchr_bench);

        // Cleanup
        free(s);
    }
}

void bench_strcpy(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = {0};
    double samples_new[NSAMPLES] = {0};

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strcpy_bench = {
#ifdef CMP_LIBC
            .name_old = "strcpy (GNU libc 2.39)",
#else
            .name_old = "strcpy (Arm OR 23.01)",
#endif
            .name_new = "strcpy (LI-PaRAD)",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* src = alloc(buf_sizes[b] + 1);
        char* dst = alloc(buf_sizes[b] + 1);
        init_buf_rand(buf_sizes[b], src, true);

#ifdef DEBUG
        fprintf(stderr, "Checking `new_strcpy_aarch64_sve`\n");
        new_strcpy_aarch64_sve(dst, src);
        assert(strcmp(src, dst) == 0 && "`new_strcpy_aarch64_sve` failed");
#endif

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            (void)strcpy(dst, src);
        }

        // Run benchmark
#ifdef CMP_LIBC
        driver_strcpy(NSAMPLES, bench_reps[b], samples_old, strcpy, dst, src);
#else
        driver_strcpy(NSAMPLES, bench_reps[b], samples_old, __strcpy_aarch64_sve, dst, src);
#endif
        driver_strcpy(NSAMPLES, bench_reps[b], samples_new, new_strcpy_aarch64_sve, dst, src);

        // Process and display results
        bench_process(&strcpy_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strcpy_bench);

        // Cleanup
        free(src);
        free(dst);
    }
}

void bench_strncpy(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = {0};
    double samples_new[NSAMPLES] = {0};

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strncpy_bench = {
#ifdef CMP_LIBC
            .name_old = "strncpy (GNU libc 2.39)",
#else
            .name_old = "strncpy (Arm OR 23.01)",
#endif
            .name_new = "strncpy (LI-PaRAD)",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* src = alloc(buf_sizes[b] + 1);
        char* dst = alloc(buf_sizes[b] + 1);
        init_buf_rand(buf_sizes[b], src, true);

#ifdef DEBUG
        fprintf(stderr, "Checking `new_strncpy_aarch64_sve`\n");
        new_strncpy_aarch64_sve(dst, src, buf_sizes[b]);
        assert(strncmp(src, dst, buf_sizes[b]) == 0 && "`new_strncpy_aarch64_sve` failed");
#endif

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            (void)strncpy(dst, src, buf_sizes[b]);
        }

        // Run benchmark
        driver_strncpy(NSAMPLES, bench_reps[b], samples_old, strncpy, dst, src, buf_sizes[b]);
        driver_strncpy(NSAMPLES, bench_reps[b], samples_new, new_strncpy_aarch64_sve, dst, src, buf_sizes[b]);

        // Process and display results
        bench_process(&strncpy_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strncpy_bench);

        // Cleanup
        free(src);
        free(dst);
    }
}

void bench_strlen(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = {0};
    double samples_new[NSAMPLES] = {0};

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strlen_bench = {
#ifdef CMP_LIBC
            .name_old = "strlen (GNU libc 2.39)",
#else
            .name_old = "strlen (Arm OR 23.01)",
#endif
            .name_new = "strlen (LI-PaRAD)",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* s = alloc(buf_sizes[b] + 1);
        init_buf_rand(buf_sizes[b], s, true);

#ifdef DEBUG
        fprintf(stderr, "Checking `new_strlen_aarch64_sve`\n");
        assert(new_strlen_aarch64_sve(s) == buf_sizes[b] && "`new_strlen_aarch64_sve` failed");
#endif

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            (void)strlen(s);
        }

        // Run benchmark
#ifdef CMP_LIBC
        driver_strlen(NSAMPLES, bench_reps[b], samples_old, strlen, s);
#else
        driver_strlen(NSAMPLES, bench_reps[b], samples_old, __strlen_aarch64_sve, s);
#endif
        driver_strlen(NSAMPLES, bench_reps[b], samples_new, new_strlen_aarch64_sve, s);

        // Process and display results
        bench_process(&strlen_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strlen_bench);

        // Cleanup
        free(s);
    }
}

void bench_strnlen(size_t nbench, size_t const buf_sizes[nbench], size_t const bench_reps[nbench]) {
    double samples_old[NSAMPLES] = {0};
    double samples_new[NSAMPLES] = {0};

    for (size_t b = 0; b < nbench; ++b) {
        // Benchmark initialization
        benchmark_t strnlen_bench = {
#ifdef CMP_LIBC
            .name_old = "strnlen (GNU libc 2.39)",
#else
            .name_old = "strnlen (Arm OR 23.01)",
#endif
            .name_new = "strnlen (LI-PaRAD)",
            .nsamples = NSAMPLES,
            .nreps = bench_reps[b],
            .buf_size = buf_sizes[b],
        };

        // Random ASCII initialization
        char* s = alloc(buf_sizes[b] + 1);
        init_buf_rand(buf_sizes[b], s, true);

#ifdef DEBUG
        fprintf(stderr, "Checking `new_strnlen_aarch64_sve`\n");
        assert(
            new_strnlen_aarch64_sve(s, buf_sizes[b] * 2) == buf_sizes[b] &&
            "`new_strnlen_aarch64_sve` failed"
        );
#endif

        // Warmup runs
        size_t const warmup_cnt = determine_warmup_cnt(bench_reps[b]);
        for (size_t i = 0; i < warmup_cnt; ++i) {
            (void)strnlen(s, buf_sizes[b]);
        }

        // Run benchmark
#ifdef CMP_LIBC
        driver_strnlen(NSAMPLES, bench_reps[b], samples_old, strnlen, s, buf_sizes[b]);
#else
        driver_strnlen(NSAMPLES, bench_reps[b], samples_old, __strnlen_aarch64_sve, s, buf_sizes[b]);
#endif
        driver_strnlen(NSAMPLES, bench_reps[b], samples_new, new_strnlen_aarch64_sve, s, buf_sizes[b]);

        // Process and display results
        bench_process(&strnlen_bench, NSAMPLES, samples_old, samples_new);
        bench_print(&strnlen_bench);

        // Cleanup
        free(s);
    }
}

int32_t main(int32_t argc, char *argv[argc + 1]) {
    // Feel free to add additional sizes here
#if defined(SMALL_STR)
    size_t buf_sizes[MAX_LEN_IN_BYTES];
    size_t bench_reps[MAX_LEN_IN_BYTES];
    for (size_t i = 0; i < MAX_LEN_IN_BYTES; ++i) {
        buf_sizes[i] = i + 1;
        bench_reps[i] = 1000000;
    };
#elif defined(FULL_SIZE_RANGE)
    size_t buf_sizes[] = {
        64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288,
        1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456,
        536870912,
    };
    size_t bench_reps[] = {
        1000000, 1000000, 1000000, 100000, 100000, 100000, 10000, 10000, 10000, 1000, 1000, 1000,
        100, 100, 100, 10, 10, 10, 2, 2, 2, 2, 2, 2,
    };
#endif

    size_t const nbench = sizeof(buf_sizes) / sizeof(buf_sizes[0]);
    // Safety check
    assert(
        sizeof(buf_sizes) == sizeof(bench_reps) &&
        "Number of buffer sizes and bench repetitions don't match"
    );

    static struct option longopts[] = {
        {"memcmp",  no_argument, 0, 'm'},
        {"memcpy",  no_argument, 0, 'x'},
        {"strcmp",  no_argument, 0, 'e'},
        {"strncmp", no_argument, 0, 'p'},
        {"strchr",  no_argument, 0, 's'},
        {"strrchr", no_argument, 0, 'r'},
        {"strcpy",  no_argument, 0, 'c'},
        {"strncpy", no_argument, 0, 'y'},
        {"strlen",  no_argument, 0, 'l'},
        {"strnlen", no_argument, 0, 'n'},
        {"help",    no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {0,         0,           0, 0},
    };

    while (true) {
        int32_t optidx = 0;
        int32_t opt = getopt_long(argc, argv, "mxepsrcylnhv", longopts, &optidx);
        if (opt == -1) {
            break;
        }

        switch (opt) {
            case 'm':
                bench_memcmp(nbench, buf_sizes, bench_reps);
                break;
            case 'x':
                bench_memcpy(nbench, buf_sizes, bench_reps);
                break;
            case 'e':
                bench_strcmp(nbench, buf_sizes, bench_reps);
                break;
            case 'p':
                bench_strncmp(nbench, buf_sizes, bench_reps);
                break;
            case 's':
                bench_strchr(nbench, buf_sizes, bench_reps);
                break;
            case 'r':
                bench_strrchr(nbench, buf_sizes, bench_reps);
                break;
            case 'c':
                bench_strcpy(nbench, buf_sizes, bench_reps);
                break;
            case 'y':
                bench_strncpy(nbench, buf_sizes, bench_reps);
                break;
            case 'l':
                bench_strlen(nbench, buf_sizes, bench_reps);
                break;
            case 'n':
                bench_strnlen(nbench, buf_sizes, bench_reps);
                break;
            case 'h':
                help();
                exit(0);
            case 'v':
                version();
                exit(0);
            default:
                fprintf(stderr, "USAGE:\n\t%s <ROUTINE>\n\n", argv[0]);
                exit(1);
        }
    }

    return 0;
}
#else
#error "Target CPU must support the (Arm OR 23.01)"
#endif
