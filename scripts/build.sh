#!/bin/bash

# Setup
cmake -S . -B build/arm-shrt-noalign -DCMAKE_C_FLAGS=''
cmake -S . -B build/arm-shrt-align   -DCMAKE_C_FLAGS='-DALIGNED_ALLOCS'
cmake -S . -B build/arm-full-noalign -DCMAKE_C_FLAGS='-DFULL_SIZE_RANGE'
cmake -S . -B build/arm-full-align   -DCMAKE_C_FLAGS='-DALIGNED_ALLOCS -DFULL_SIZE_RANGE'
cmake -S . -B build/gnu-shrt-noalign -DCMAKE_C_FLAGS='-DCMP_LIBC'
cmake -S . -B build/gnu-shrt-align   -DCMAKE_C_FLAGS='-DCMP_LIBC -DALIGNED_ALLOCS'
cmake -S . -B build/gnu-full-noalign -DCMAKE_C_FLAGS='-DCMP_LIBC -DFULL_SIZE_RANGE'
cmake -S . -B build/gnu-full-align   -DCMAKE_C_FLAGS='-DCMP_LIBC -DALIGNED_ALLOCS -DFULL_SIZE_RANGE'

# Build
cmake --build build/arm-shrt-noalign -j 4
cmake --build build/arm-shrt-align   -j 4
cmake --build build/arm-full-noalign -j 4
cmake --build build/arm-full-align   -j 4
cmake --build build/gnu-shrt-noalign -j 4
cmake --build build/gnu-shrt-align   -j 4
cmake --build build/gnu-full-noalign -j 4
cmake --build build/gnu-full-align   -j 4

