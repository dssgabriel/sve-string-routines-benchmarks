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

#include "bench.h"
#include "stats.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

void bench_process(
    benchmark_t self[static 1], size_t nsamples, double old[nsamples], double new[nsamples]
) {
    qsort(old, nsamples, sizeof(double), cmp_double);
    self->old_stats.min = old[0];
    self->old_stats.med = old[nsamples / 2 + 1];
    self->old_stats.max = old[nsamples - 1];
    self->old_stats.avg = mean(nsamples, old);
    self->old_stats.err = stddev(nsamples, old, self->old_stats.avg) * 100.0 / self->old_stats.avg;

    qsort(new, nsamples, sizeof(double), cmp_double);
    self->new_stats.min = new[0];
    self->new_stats.med = new[nsamples / 2 + 1];
    self->new_stats.max = new[nsamples - 1];
    self->new_stats.avg = mean(nsamples, new);
    self->new_stats.err = stddev(nsamples, new, self->new_stats.avg) * 100.0 / self->new_stats.avg;

    self->speedup = self->old_stats.avg / self->new_stats.avg;
}

void bench_print(benchmark_t self[static 1]) {
    static bool header = false;
    if (!header) {
        printf(
            "%20s |%12s |%15s |%15s |%15s |%15s |%12s |%12s |%12s\n",
            "BENCHMARK NAME",
            "SIZE",
            "MINIMUM ns",
            "MEDIAN ns",
            "MAXIMUM ns",
            "AVERAGE ns",
            "ERR %",
            "BW GiB/s",
            "SPEEDUP %"
        );
        for (size_t i = 0; i < 17 * 4 + 14 * 4 + 22; ++i) { printf("-"); }
        printf("\n");
        header = true;
    }

    double array_size = (double)self->buf_size;
    char* unit[] = { "B", "KiB", "MiB", "GiB", "TiB" };
    uint8_t unit_selector = 0;
    while (array_size >= 1024.0) {
        array_size /= 1024.0;
       unit_selector += 1;
    }
    char fmt_array_size[13];
    sprintf(fmt_array_size, "%8.1lf %-3s", array_size, unit[unit_selector]);
    if (fmt_array_size[10] != 'i') { fmt_array_size[10] = '\0'; }
    fmt_array_size[12] = '\0';

    printf(
        "%20s |%12s |%15.0lf |%15.0lf |%15.0lf |%15.3lf |%12.3lf |%12.3lf |%12s\n",
        self->old_name,
        fmt_array_size,
        self->old_stats.min,
        self->old_stats.med,
        self->old_stats.max,
        self->old_stats.avg,
        self->old_stats.err,
        ((double)self->buf_size / (double)(1024 << 20)) / ns_to_s(self->old_stats.avg),
        ""
    );
    printf(
        "%20s |%12s |%15.0lf |%15.0lf |%15.0lf |%15.3lf |%12.3lf |%12.3lf |%+11.1lf%%\n",
        self->new_name,
        fmt_array_size,
        self->new_stats.min,
        self->new_stats.med,
        self->new_stats.max,
        self->new_stats.avg,
        self->new_stats.err,
        ((double)self->buf_size / (double)(1024 << 20)) / ns_to_s(self->new_stats.avg),
        (self->speedup - 1.0) * 100.0
    );

    for (size_t i = 0; i < 17 * 4 + 14 * 4 + 22; ++i) { printf("-"); }
    printf("\n");
}


