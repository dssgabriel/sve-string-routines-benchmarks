#!/bin/bash

RAW_SHRT_NOALIGN=results/raw/short_strs/noalign
RAW_SHRT_ALIGN=results/raw/short_strs/align
RAW_FULL_NOALIGN=results/raw/full_sizes/noalign
RAW_FULL_ALIGN=results/raw/full_sizes/align

PLT_SHRT_NOALIGN=results/plots/short_strs/noalign
PLT_SHRT_ALIGN=results/plots/short_strs/align
PLT_FULL_NOALIGN=results/plots/full_sizes/noalign
PLT_FULL_ALIGN=results/plots/full_sizes/align

routines=(memcmp strcmp strncmp memcpy strcpy strncpy strchr strrchr strlen strnlen)

for r in ${routines[*]}; do
    python3 scripts/plot_bw.py \
        --input $RAW_SHRT_NOALIGN/${r}_libc.dat \
        --output $PLT_SHRT_NOALIGN/${r}_libc.png \
        --target G3E \
        --routine $r &

    python3 scripts/plot_bw.py \
        --input $RAW_SHRT_ALIGN/${r}_libc.dat \
        --output $PLT_SHRT_ALIGN/${r}_libc.png \
        --target G3E \
        --routine $r \
        --aligned-alloc &

    python3 scripts/plot_bw.py \
        --input $RAW_FULL_NOALIGN/${r}_libc.dat \
        --output $PLT_FULL_NOALIGN/${r}_libc.png \
        --target G3E \
        --routine $r \
        --full-sizes &

    python3 scripts/plot_bw.py \
        --input $RAW_FULL_ALIGN/${r}_libc.dat \
        --output $PLT_FULL_ALIGN/${r}_libc.png \
        --target G3E \
        --routine $r \
        --full-sizes \
        --aligned-alloc &
done
