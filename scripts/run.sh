#!/bin/bash

BIN=bench-sve-string-routines
routines=(memcmp strcmp strncmp memcpy strcpy strncpy strchr strrchr strlen strnlen)

for m in (shrt full); do
    for a in (align noalign); do
        for i in ${!routines[@]}; do
            j=$(($i + ${#routines[@]}))

            taskset -c $i build/gnu-${m}-${a}/$BIN --${routines[$i]} > results/raw/${m}/${a}/${routines[$i]}-gnu.dat &
            taskset -c $j build/arm-${m}-${a}/$BIN --${routines[$i]} > results/raw/${m}/${a}/${routines[$i]}-arm.dat &
        done
    done
done
