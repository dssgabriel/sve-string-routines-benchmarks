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
    qsort(new, nsamples, sizeof(double), cmp_double);
    self->rt_old.min = old[0];
    self->rt_old.med = old[nsamples / 2 + 1];
    self->rt_old.max = old[nsamples - 1];
    self->rt_old.avg = mean(nsamples, old);
    self->rt_old.err = stddev(nsamples, old, self->rt_old.avg) * 100.0 / self->rt_old.avg;
    self->rt_new.min = new[0];
    self->rt_new.med = new[nsamples / 2 + 1];
    self->rt_new.max = new[nsamples - 1];
    self->rt_new.avg = mean(nsamples, new);
    self->rt_new.err = stddev(nsamples, new, self->rt_new.avg) * 100.0 / self->rt_new.avg;
    self->rt_speedup = self->rt_old.avg / self->rt_new.avg;

    double bw_old[nsamples];
    double bw_new[nsamples];
    double buf_size_gib = (double)self->buf_size / ONE_GIB;
    for (size_t i = 0; i < nsamples; ++i) {
        bw_old[i] = buf_size_gib / ns_to_s(old[i]);
        bw_new[i] = buf_size_gib / ns_to_s(new[i]);
    }
    qsort(bw_old, nsamples, sizeof(double), cmp_double);
    qsort(bw_new, nsamples, sizeof(double), cmp_double);
    self->bw_old.min = bw_old[0];
    self->bw_old.med = bw_old[nsamples / 2 + 1];
    self->bw_old.max = bw_old[nsamples - 1];
    self->bw_old.avg = mean(nsamples, bw_old);
    self->bw_old.err = stddev(nsamples, bw_old, self->bw_old.avg);
    self->bw_new.min = bw_new[0];
    self->bw_new.med = bw_new[nsamples / 2 + 1];
    self->bw_new.max = bw_new[nsamples - 1];
    self->bw_new.avg = mean(nsamples, bw_new);
    self->bw_new.err = stddev(nsamples, bw_new, self->bw_new.avg);
    self->bw_speedup = 1.0 / (self->bw_old.avg / self->bw_new.avg);
}

static inline void print_line() {
    for (size_t i = 0; i < 17 * 7 + 14 * 2 + 25; ++i) { printf("-"); }
    printf("\n");
}

void bench_print(benchmark_t const self[static 1]) {
    static bool header = false;
    if (!header) {
        printf(
            "%24s |%12s |%15s |%15s |%15s |%15s |%15s |%15s |%15s |%12s\n",
            "ROUTINE IMPLEMENTATION", "BUF SIZE",
            "RT MIN ns", "RT MED ns", "RT MAX ns", "RT AVG ns", "RT STDEV %",
            "BW AVG GiB/s", "BW STDEV GiB/s",
            "SPEEDUP"
        );
        header = true;
    }
    print_line();

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
        "%24s |%12s |%15.0lf |%15.0lf |%15.0lf |%15.3lf |%15.3lf |%15.3lf |%15.3lf |\n",
        self->name_old, fmt_array_size,
        self->rt_old.min, self->rt_old.med, self->rt_old.max, self->rt_old.avg, self->rt_old.err,
        self->bw_old.avg, self->bw_old.err
    );
    printf(
        "%24s |%12s |%15.0lf |%15.0lf |%15.0lf |%15.3lf |%15.3lf |%15.3lf |%15.3lf |%+11.2lf%%\n",
        self->name_new, fmt_array_size,
        self->rt_new.min, self->rt_new.med, self->rt_new.max, self->rt_new.avg, self->rt_new.err,
        self->bw_new.avg, self->bw_new.err,
        (self->rt_speedup - 1.0) * 100.0
    );
}
