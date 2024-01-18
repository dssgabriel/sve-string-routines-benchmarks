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

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#define NSAMPLES 31ULL

void bench_memcmp(size_t nb, size_t const buf_sizes[nb], size_t const bench_iters[nb]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nb; ++b) {
        // Benchmark initialization
        benchmark_t bench = {
            .old_name = "memcmp_sve",
            .new_name = "new_memcmp_sve",
            .nsamples = NSAMPLES,
            .niters = bench_iters[b],
            .buf_size = buf_sizes[b],
        };

        char* s1 = malloc(buf_sizes[b] + 1);
        char* s2 = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            s1[c] = rand() % 256;
            s2[c] = s1[c]; // Make strings identical
        }
        s1[buf_sizes[b]] = '\0';
        s2[buf_sizes[b]] = '\0';

        // Warmup
        size_t warmup_cnt = 1;
        if (buf_sizes[b] < 262144) {
            warmup_cnt = 100;
        } else if (buf_sizes[b] < 16777216) {
            warmup_cnt = 10;
        }
        for (size_t i = 0; i < warmup_cnt; ++i) {
            memcmp(s1, s2, buf_sizes[b]);
        }

        // Run benchmark
        driver_memcmp(NSAMPLES, bench_iters[b], samples_old, __memcmp_aarch64_sve, s1, s2, buf_sizes[b]);
        driver_memcmp(NSAMPLES, bench_iters[b], samples_new, new_memcmp_aarch64_sve, s1, s2, buf_sizes[b]);

        // Process and display results
        bench_process(&bench, NSAMPLES, samples_old, samples_new);
        bench_print(&bench);

        // Cleanup
        free(s1);
        free(s2);
    }
}

void bench_strchr(size_t nb, size_t const buf_sizes[nb], size_t const bench_iters[nb]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nb; ++b) {
        // Benchmark initialization
        benchmark_t bench = {
            .old_name = "strchr_sve",
            .new_name = "new_strchr_sve",
            .nsamples = NSAMPLES,
            .niters = bench_iters[b],
            .buf_size = buf_sizes[b],
        };

        char* s = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            s[c] = rand() % 95 + 32;
        }
        s[buf_sizes[b]] = '\0';
        int32_t c = 0;

        // Warmup
        size_t warmup_cnt = 1;
        if (buf_sizes[b] < 262144) {
            warmup_cnt = 100;
        } else if (buf_sizes[b] < 16777216) {
            warmup_cnt = 10;
        }
        for (size_t i = 0; i < warmup_cnt; ++i) {
            strchr(s, c);
        }

        // Run benchmark
        driver_strchr(NSAMPLES, bench_iters[b], samples_old, __strchr_aarch64_sve, s, c);
        driver_strchr(NSAMPLES, bench_iters[b], samples_new, new_strchr_aarch64_sve, s, c);

        // Process and display results
        bench_process(&bench, NSAMPLES, samples_old, samples_new);
        bench_print(&bench);

        // Cleanup
        free(s);
    }
}

void bench_strcmp(size_t nb, size_t const buf_sizes[nb], size_t const bench_iters[nb]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nb; ++b) {
        // Benchmark initialization
        benchmark_t bench = {
            .old_name = "strcmp_sve",
            .new_name = "new_strcmp_sve",
            .nsamples = NSAMPLES,
            .niters = bench_iters[b],
            .buf_size = buf_sizes[b],
        };

        char* s1 = malloc(buf_sizes[b] + 1);
        char* s2 = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            s1[c] = rand() % 95 + 32;
            s2[c] = s1[c]; // Make strings identical
        }
        s1[buf_sizes[b]] = '\0';
        s2[buf_sizes[b]] = '\0';

        // Warmup
        size_t warmup_cnt = 1;
        if (buf_sizes[b] < 262144) {
            warmup_cnt = 100;
        } else if (buf_sizes[b] < 16777216) {
            warmup_cnt = 10;
        }
        for (size_t i = 0; i < warmup_cnt; ++i) {
            strcmp(s1, s2);
        }

        // Run benchmark
        driver_strcmp(NSAMPLES, bench_iters[b], samples_old, __strcmp_aarch64_sve, s1, s2);
        driver_strcmp(NSAMPLES, bench_iters[b], samples_new, new_strcmp_aarch64_sve, s1, s2);

        // Process and display results
        bench_process(&bench, NSAMPLES, samples_old, samples_new);
        bench_print(&bench);

        // Cleanup
        free(s1);
        free(s2);
    }
}

void bench_strcpy(size_t nb, size_t const buf_sizes[nb], size_t const bench_iters[nb]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nb; ++b) {
        // Benchmark initialization
        benchmark_t bench = {
            .old_name = "strcpy_sve",
            .new_name = "new_strcpy_sve",
            .nsamples = NSAMPLES,
            .niters = bench_iters[b],
            .buf_size = buf_sizes[b],
        };

        char* src = malloc(buf_sizes[b] + 1);
        char* dst = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            src[c] = rand() % 95 + 32;
        }
        src[buf_sizes[b]] = '\0';
        dst[buf_sizes[b]] = '\0';

        // Warmup
        size_t warmup_cnt = 1;
        if (buf_sizes[b] < 262144) {
            warmup_cnt = 100;
        } else if (buf_sizes[b] < 16777216) {
            warmup_cnt = 10;
        }
        for (size_t i = 0; i < warmup_cnt; ++i) {
            strcpy(dst, src);
        }

        // Run benchmark
        driver_strcpy(NSAMPLES, bench_iters[b], samples_old, __strcpy_aarch64_sve, dst, src);
        driver_strcpy(NSAMPLES, bench_iters[b], samples_new, new_strcpy_aarch64_sve, dst, src);

        // Process and display results
        bench_process(&bench, NSAMPLES, samples_old, samples_new);
        bench_print(&bench);

        // Cleanup
        free(src);
        free(dst);
    }
}

void bench_strlen(size_t nb, size_t const buf_sizes[nb], size_t const bench_iters[nb]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nb; ++b) {
        // Benchmark initialization
        benchmark_t bench = {
            .old_name = "strlen_sve",
            .new_name = "new_strlen_sve",
            .nsamples = NSAMPLES,
            .niters = bench_iters[b],
            .buf_size = buf_sizes[b],
        };

        char* s = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            s[c] = rand() % 95 + 32;
        }
        s[buf_sizes[b]] = '\0';

        // Warmup
        size_t warmup_cnt = 1;
        if (buf_sizes[b] < 262144) {
            warmup_cnt = 100;
        } else if (buf_sizes[b] < 16777216) {
            warmup_cnt = 10;
        }
        for (size_t i = 0; i < warmup_cnt; ++i) {
            strlen(s);
        }

        // Run benchmark
        driver_strlen(NSAMPLES, bench_iters[b], samples_old, __strlen_aarch64_sve, s);
        driver_strlen(NSAMPLES, bench_iters[b], samples_new, new_strlen_aarch64_sve, s);

        // Process and display results
        bench_process(&bench, NSAMPLES, samples_old, samples_new);
        bench_print(&bench);

        // Cleanup
        free(s);
    }
}

void bench_strncmp(size_t nb, size_t const buf_sizes[nb], size_t const bench_iters[nb]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nb; ++b) {
        // Benchmark initialization
        benchmark_t bench = {
            .old_name = "strncmp_sve",
            .new_name = "new_strncmp_sve",
            .nsamples = NSAMPLES,
            .niters = bench_iters[b],
            .buf_size = buf_sizes[b],
        };

        char* s1 = malloc(buf_sizes[b] + 1);
        char* s2 = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            s1[c] = rand() % 95 + 32;
            s2[c] = s1[c]; // Make strings identical
        }
        s1[buf_sizes[b]] = '\0';
        s2[buf_sizes[b]] = '\0';

        // Warmup
        size_t warmup_cnt = 1;
        if (buf_sizes[b] < 262144) {
            warmup_cnt = 100;
        } else if (buf_sizes[b] < 16777216) {
            warmup_cnt = 10;
        }
        for (size_t i = 0; i < warmup_cnt; ++i) {
            strncmp(s1, s2, buf_sizes[b]);
        }

        // Run benchmark
        driver_strncmp(NSAMPLES, bench_iters[b], samples_old, __strncmp_aarch64_sve, s1, s2, buf_sizes[b]);
        driver_strncmp(NSAMPLES, bench_iters[b], samples_new, new_strncmp_aarch64_sve, s1, s2, buf_sizes[b]);

        // Process and display results
        bench_process(&bench, NSAMPLES, samples_old, samples_new);
        bench_print(&bench);

        // Cleanup
        free(s1);
        free(s2);
    }
}

void bench_strnlen(size_t nb, size_t const buf_sizes[nb], size_t const bench_iters[nb]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nb; ++b) {
        // Benchmark initialization
        benchmark_t bench = {
            .old_name = "strnlen_sve",
            .new_name = "new_strnlen_sve",
            .nsamples = NSAMPLES,
            .niters = bench_iters[b],
            .buf_size = buf_sizes[b],
        };

        char* s = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            s[c] = rand() % 95 + 32;
        }
        s[buf_sizes[b]] = '\0';

        // Warmup
        size_t warmup_cnt = 1;
        if (buf_sizes[b] < 262144) {
            warmup_cnt = 100;
        } else if (buf_sizes[b] < 16777216) {
            warmup_cnt = 10;
        }
        for (size_t i = 0; i < warmup_cnt; ++i) {
            strnlen(s, buf_sizes[b]);
        }

        // Run benchmark
        driver_strnlen(NSAMPLES, bench_iters[b], samples_old, __strnlen_aarch64_sve, s, buf_sizes[b]);
        driver_strnlen(NSAMPLES, bench_iters[b], samples_new, new_strnlen_aarch64_sve, s, buf_sizes[b]);

        // Process and display results
        bench_process(&bench, NSAMPLES, samples_old, samples_new);
        bench_print(&bench);

        // Cleanup
        free(s);
    }
}

void bench_strrchr(size_t nb, size_t const buf_sizes[nb], size_t const bench_iters[nb]) {
    double samples_old[NSAMPLES] = { 0 };
    double samples_new[NSAMPLES] = { 0 };

    for (size_t b = 0; b < nb; ++b) {
        // Benchmark initialization
        benchmark_t bench = {
            .old_name = "strrchr_sve",
            .new_name = "new_strrchr_sve",
            .nsamples = NSAMPLES,
            .niters = bench_iters[b],
            .buf_size = buf_sizes[b],
        };

        char* s = malloc(buf_sizes[b] + 1);
        for (size_t c = 0; c < buf_sizes[b]; ++c) {
            s[c] = rand() % 95 + 32;
        }
        s[buf_sizes[b]] = '\0';
        int32_t c = 0;

        // Warmup
        size_t warmup_cnt = 1;
        if (buf_sizes[b] < 262144) {
            warmup_cnt = 100;
        } else if (buf_sizes[b] < 16777216) {
            warmup_cnt = 10;
        }
        for (size_t i = 0; i < warmup_cnt; ++i) {
            strrchr(s, c);
        }

        // Run benchmark
        driver_strrchr(NSAMPLES, bench_iters[b], samples_old, __strrchr_aarch64_sve, s, c);
        driver_strrchr(NSAMPLES, bench_iters[b], samples_new, new_strrchr_aarch64_sve, s, c);

        // Process and display results
        bench_process(&bench, NSAMPLES, samples_old, samples_new);
        bench_print(&bench);

        // Cleanup
        free(s);
    }
}

int32_t main(int32_t argc, char* argv[argc + 1]) {
    const size_t BUF_SIZES[] = {
        32768,    // Half L1d$
        65536,    // Full L1d$
        524288,   // Half L2$
        1048576,  // Full L2$
        16777216, // Half L3$
        33554432, // Full L3$
        268435456 // DRAM
    };
    size_t NITERS[16] = { 10000, 5000, 250, 100, 10, 5, 1 };
    const size_t nbench = sizeof(BUF_SIZES) / sizeof(BUF_SIZES[0]);

    static struct option longopts[] = {
        { "memcmp",  no_argument, 0, 'm' },
        { "strchr",  no_argument, 0, 's' },
        { "strcmp",  no_argument, 0, 'e' },
        { "strcpy",  no_argument, 0, 'c' },
        { "strlen",  no_argument, 0, 'l' },
        { "strncmp", no_argument, 0, 'p' },
        { "strnlen", no_argument, 0, 'n' },
        { "strrchr", no_argument, 0, 'r' },
        { "help",    no_argument, 0, 'h' },
        { "version", no_argument, 0, 'v' },
        { 0,         0,           0, 0 }
    };

    while (true) {
        int32_t optidx = 0;
        int32_t opt = getopt_long_only(argc, argv, "mseclpnrhv", longopts, &optidx);
        if (opt == -1) { break; }

        switch (opt) {
            case 'm':
                bench_memcmp(nbench, BUF_SIZES, NITERS);
                break;
            case 's':
                bench_strchr(nbench, BUF_SIZES, NITERS);
                break;
            case 'e':
                bench_strcmp(nbench, BUF_SIZES, NITERS);
                break;
            case 'c':
                bench_strcpy(nbench, BUF_SIZES, NITERS);
                break;
            case 'l':
                bench_strlen(nbench, BUF_SIZES, NITERS);
                break;
            case 'p':
                bench_strncmp(nbench, BUF_SIZES, NITERS);
                break;
            case 'n':
                bench_strnlen(nbench, BUF_SIZES, NITERS);
                break;
            case 'r':
                bench_strrchr(nbench, BUF_SIZES, NITERS);
                break;
            case 'h':
                help(argv[0]);
                exit(0);
            case 'v':
                version(argv[0]);
                exit(0);
            default:
                exit(1);
        }
    }

    return 0;
}
