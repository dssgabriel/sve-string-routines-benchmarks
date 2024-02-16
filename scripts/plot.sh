#!/bin/bash

RESULTS=results/raw

routines=(memcmp strcmp strncmp memcpy strcpy strncpy strchr strrchr strlen strnlen)

for r in ${routines[*]}; do
    # python3 scripts/plot_bw.py -i results/raw/$r-libc.dat -o results/plots/$r-libc.png -t G3E -r $r
    # python3 scripts/plot_bw.py -i results/raw/$r-aor.dat -o results/plots/$r-aor.png -t G3E -r $r
    python3 scripts/plot_bw.py -i results/raw/$r.dat -o results/plots/$r.png -t G3E -r $r
done
