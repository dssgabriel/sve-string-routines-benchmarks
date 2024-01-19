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

#define ONE_GIB (double)(1024 << 20)

void bench_process(
    benchmark_t self[static 1], size_t nsamples, double old[nsamples], double new[nsamples]
) {
    qsort(old, nsamples, sizeof(double), cmp_double);
    self->stats_old.min = old[0];
    self->stats_old.med = old[nsamples / 2 + 1];
    self->stats_old.max = old[nsamples - 1];
    self->stats_old.avg = mean(nsamples, old);
    self->stats_old.err = stddev(nsamples, old, self->stats_old.avg) * 100.0 / self->stats_old.avg;

    qsort(new, nsamples, sizeof(double), cmp_double);
    self->stats_new.min = new[0];
    self->stats_new.med = new[nsamples / 2 + 1];
    self->stats_new.max = new[nsamples - 1];
    self->stats_new.avg = mean(nsamples, new);
    self->stats_new.err = stddev(nsamples, new, self->stats_new.avg) * 100.0 / self->stats_new.avg;

    self->speedup = self->stats_old.avg / self->stats_new.avg;
}

static inline void print_line() {
    for (size_t i = 0; i < 17 * 4 + 14 * 4 + 22; ++i) { printf("-"); }
    printf("\n");
}

void bench_print(benchmark_t const self[static 1]) {
    static bool header = false;
    if (!header) {
        printf(
            "%20s |%12s |%15s |%15s |%15s |%15s |%12s |%12s |%12s\n",
            "ROUTINE IMPLEM",
            "BUF SIZE",
            "MINIMUM ns",
            "MEDIAN ns",
            "MAXIMUM ns",
            "AVERAGE ns",
            "STD DEV %",
            "BW GiB/s",
            "SPEEDUP %"
        );
        print_line();
        header = true;
    }

    double array_size = (double)self->buf_size;
    char* const unit[] = { "B", "KiB", "MiB", "GiB", "TiB" };
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
        "%20s |%12s |%15.0lf |%15.0lf |%15.0lf |%15.3lf |%12.3lf |%12.3lf |\n",
        self->name_old,
        fmt_array_size,
        self->stats_old.min,
        self->stats_old.med,
        self->stats_old.max,
        self->stats_old.avg,
        self->stats_old.err,
        ((double)self->buf_size / ONE_GIB) / ns_to_s(self->stats_old.avg)
    );
    printf(
        "%20s |%12s |%15.0lf |%15.0lf |%15.0lf |%15.3lf |%12.3lf |%12.3lf |%+10.2lf%%\n",
        self->name_new,
        fmt_array_size,
        self->stats_new.min,
        self->stats_new.med,
        self->stats_new.max,
        self->stats_new.avg,
        self->stats_new.err,
        ((double)self->buf_size / ONE_GIB) / ns_to_s(self->stats_new.avg),
        (self->speedup - 1.0) * 100.0
    );
    print_line();
}
