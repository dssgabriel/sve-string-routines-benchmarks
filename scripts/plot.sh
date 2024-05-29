#!/bin/bash

ARCH=$1

RAW_SHRT_NOALIGN=results/raw/$ARCH/shrt/noalign
RAW_SHRT_ALIGN=results/raw/$ARCH/shrt/align
RAW_FULL_NOALIGN=results/raw/$ARCH/full/noalign
RAW_FULL_ALIGN=results/raw/$ARCH/full/align

PLT_SHRT_NOALIGN=results/plots/$ARCH/shrt/noalign
PLT_SHRT_ALIGN=results/plots/$ARCH/shrt/align
PLT_FULL_NOALIGN=results/plots/$ARCH/full/noalign
PLT_FULL_ALIGN=results/plots/$ARCH/full/align

routines=(memcmp strcmp strncmp memcpy strcpy strncpy strchr strrchr strlen strnlen)

for r in ${routines[*]}; do
    python3 scripts/plot_bw.py \
        --input $RAW_SHRT_NOALIGN/${r}.dat \
        --output $PLT_SHRT_NOALIGN/${r}.png \
        --target $ARCH \
        --routine $r &
    python3 scripts/plot_bw.py \
        --input $RAW_SHRT_ALIGN/${r}.dat \
        --output $PLT_SHRT_ALIGN/${r}.png \
        --target $ARCH \
        --routine $r \
        --aligned-alloc &
    python3 scripts/plot_bw.py \
        --input $RAW_FULL_NOALIGN/${r}.dat \
        --output $PLT_FULL_NOALIGN/${r}.png \
        --target $ARCH \
        --routine $r \
        --full-sizes &
    python3 scripts/plot_bw.py \
        --input $RAW_FULL_ALIGN/${r}.dat \
        --output $PLT_FULL_ALIGN/${r}.png \
        --target $ARCH \
        --routine $r \
        --full-sizes \
        --aligned-alloc &
done
