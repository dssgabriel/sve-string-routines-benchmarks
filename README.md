# Comparative benchmarks for Arm SVE-optimized string routines


## About

This repository benchmarks the current implementations of some SVE-optimized string routines from [Arm's optimized-routines library](https://github.com/ARM-software/optimized-routines) to alternate implementations.

This project was done as part of the EMOPASS European project and conducted at the University of Versailles --- Saint-Quentin-en-Yvelines by the LI-PaRAD computer science lab, in collaboration with SiPearl and Atos/Eviden.

The new implementations replaces `INCx` SVE instructions by a combination of `CNTx` (in the loop prelude) and `ADD` (in the loop). Performance benchmarks in the context of HPC applications on Arm Neoverse V1 microarchitectures have shown that there are noticeable penalty when using `INCx` instructions (predicated or not) to increment the loop counter.
This optimization is only used in paths where the vector is known to be whole, and where computing the width of an entire vector in advance (using `CNTx`) guarantees that no elements are ignored.

These optimizations have specifically been tested on a Neoverse V1 microarchitecture (AWS Graviton3) and have shown to yield compelling speedups on all routines for string sizes ranging from 32 KiB (half of the size of the L1d cache) to 256 MiB (8x the size of the L3 cache). Detailed results are presented in the [results](#results) section at the end of this README.


## Build & run

### Pre-requisites

- CMake 3.16
- C11 compiler

### Building

```sh
cmake -S . -B build
cmake --build build
```

### Running

```sh
./build/bench-sve-string-routines --help
```


## Benchmark methodology

TODO


## Results

The raw results are available in the `results/` directory.

TODO: Add plots
