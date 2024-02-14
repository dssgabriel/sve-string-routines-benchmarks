#!/bin/bash

LIBC_BIN=build/release-libc/bench-sve-string-routines
AOR_BIN=build/release-arm/bench-sve-string-routines

routines=(memcmp strcmp strncmp memcpy strcpy strncpy strchr strrchr strlen strnlen)

for i in ${!routines[@]}; do
    j=$(($i + ${#routines[@]}))
    taskset -c $i $LIBC_BIN --${routines[$i]} > results/raw/$r-libc.dat &
    taskset -c $j $AOR_BIN --${routines[$i]} > results/raw/$r-aor.dat &
done
