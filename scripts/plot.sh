#!/bin/bash

RAW_SHRT_NOALIGN=results/raw/shrt/noalign
RAW_SHRT_ALIGN=results/raw/shrt/align
RAW_FULL_NOALIGN=results/raw/full/noalign
RAW_FULL_ALIGN=results/raw/full/align

PLT_SHRT_NOALIGN=results/plots/shrt/noalign
PLT_SHRT_ALIGN=results/plots/shrt/align
PLT_FULL_NOALIGN=results/plots/full/noalign
PLT_FULL_ALIGN=results/plots/full/align

routines=(memcmp strcmp strncmp memcpy strcpy strncpy strchr strrchr strlen strnlen)

for r in ${routines[*]}; do
    # python3 scripts/plot_bw.py \
    #     --input $RAW_SHRT_NOALIGN/${r}-gnu.dat \
    #     --output $PLT_SHRT_NOALIGN/${r}-gnu.png \
    #     --target G3E \
    #     --routine $r &
    # python3 scripts/plot_bw.py \
    #     --input $RAW_SHRT_ALIGN/${r}-gnu.dat \
    #     --output $PLT_SHRT_ALIGN/${r}-gnu.png \
    #     --target G3E \
    #     --routine $r \
    #     --aligned-alloc &
    # python3 scripts/plot_bw.py \
    #     --input $RAW_FULL_NOALIGN/${r}-gnu.dat \
    #     --output $PLT_FULL_NOALIGN/${r}-gnu.png \
    #     --target G3E \
    #     --routine $r \
    #     --full-sizes &
    # python3 scripts/plot_bw.py \
    #     --input $RAW_FULL_ALIGN/${r}-gnu.dat \
    #     --output $PLT_FULL_ALIGN/${r}-gnu.png \
    #     --target G3E \
    #     --routine $r \
    #     --full-sizes \
    #     --aligned-alloc &

    # python3 scripts/plot_bw.py \
    #     --input $RAW_SHRT_NOALIGN/${r}-aor.dat \
    #     --output $PLT_SHRT_NOALIGN/${r}-aor.png \
    #     --target G3E \
    #     --routine $r &
    # python3 scripts/plot_bw.py \
    #     --input $RAW_SHRT_ALIGN/${r}-aor.dat \
    #     --output $PLT_SHRT_ALIGN/${r}-aor.png \
    #     --target G3E \
    #     --routine $r \
    #     --aligned-alloc &
    # python3 scripts/plot_bw.py \
    #     --input $RAW_FULL_NOALIGN/${r}-aor.dat \
    #     --output $PLT_FULL_NOALIGN/${r}-aor.png \
    #     --target G3E \
    #     --routine $r \
    #     --full-sizes &
    # python3 scripts/plot_bw.py \
    #     --input $RAW_FULL_ALIGN/${r}-aor.dat \
    #     --output $PLT_FULL_ALIGN/${r}-aor.png \
    #     --target G3E \
    #     --routine $r \
    #     --full-sizes \
    #     --aligned-alloc &

    python3 scripts/plot_bw.py \
        --input $RAW_SHRT_NOALIGN/${r}.dat \
        --output $PLT_SHRT_NOALIGN/${r}.png \
        --target G3E \
        --routine $r &
    python3 scripts/plot_bw.py \
        --input $RAW_SHRT_ALIGN/${r}.dat \
        --output $PLT_SHRT_ALIGN/${r}.png \
        --target G3E \
        --routine $r \
        --aligned-alloc &
    python3 scripts/plot_bw.py \
        --input $RAW_FULL_NOALIGN/${r}.dat \
        --output $PLT_FULL_NOALIGN/${r}.png \
        --target G3E \
        --routine $r \
        --full-sizes &
    python3 scripts/plot_bw.py \
        --input $RAW_FULL_ALIGN/${r}.dat \
        --output $PLT_FULL_ALIGN/${r}.png \
        --target G3E \
        --routine $r \
        --full-sizes \
        --aligned-alloc &
done
