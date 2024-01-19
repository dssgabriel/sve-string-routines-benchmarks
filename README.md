# Comparative benchmarks for SVE-optimized string routines


## About

This repository benchmarks the current implementations of some AArch64 SVE string routines from [Arm's optimized-routines library](https://github.com/ARM-software/optimized-routines) against newly proposed implementations.

This project was done as part of the EMOPASS European Project and conducted at the University of Versailles — Saint-Quentin-en-Yvelines (UVSQ) by the LI-PaRAD computer science lab, in collaboration with industrial partners: SiPearl and Atos/Eviden.

Performance benchmarks in the context of HPC applications on Arm Neoverse V1 microarchitectures have shown that there are noticeable penalty when using `INCx` instructions (predicated or not) to increment the loop counter, instead of using a combination of `CNTx` (in the loop prelude) and `ADD` (in the loop). The new implementations replace the `INCx` instructions in paths where the whole vector is known to be valid (i.e., the critical path in most scenarios).

These optimizations have specifically been tested on a AWS Graviton3 instance (Arm Neoverse V1 µarch) and have shown to yield compelling speedups on all modified routines for string sizes ranging from 32 KiB (half the size of the G3's L1d cache) to 256 MiB (8x the size of the G3's L3 cache). Detailed results are presented in the [Results](#results) section at the end of this README.


## Build & run

### Pre-requisites

- CMake 3.16
- C11-conformant C compiler

### Building

```sh
cmake -S . -B build
cmake --build build
```

### Running

All options can be listed with the following command:
```sh
./build/bench-sve-string-routines --help
```

Run specific routines with their name as an option, e.g., for `memcmp` and `strcpy`:
```sh
./build/bench-sve-string-routines --memcmp --strcpy
```


## Benchmark methodology

Our approach to benchmarking tries to be as straightforward, accurate, stable and reproducible as possible on different systems. All benchmark results presented in this repository were obtained on a AWS Graviton3 CPU (`c7g.medium` instance, on kernel `5.15.0-1051-aws` and using GCC 11.1.0).

For all routines, we tested the following buffer sizes:
- 32 KiB, half of the Graviton's 3 L1D cache
- 64 KiB, full L1D cache
- 512 KiB, half L2 cache
- 1 MiB, full L2 cache
- 16 MiB, half L3 cache
- 32 MiB, full L3 cache
- 256 MiB, 8x L3 cache, guaranteed to be in DDR5 RAM

Each measurement is sampled 31 times, which guarantees an accurate representation of runtime performance, and runtime stability.   
For small buffers, the execution time can be very short (less than 1 µs). In this case, the sampled execution time is obtained using an inner loop where the routine is called repeatedly and execution time is averaged over the number of repetitions.   
In order to ensure that data buffers are in cache before running the actual benchmark, a few iterations of warmup calls a performed.   
Peformance measurements are done using `clock_gettime` and the `CLOCK_MONOTONIC_RAW` clock, which offer an accurate resolution.


## Results

The raw results are available in the `results/raw` directory. These include the following metrics for each benchmarked routine:
- string buffer size
- minimum execution time
- maximum execution time
- median execution time
- average execution time
- percentage of standard deviation (i.e., stability of the run)
- bandwidth in GiB/s
- speedup (as percentage of performance improvement) 

The following plots (generated with the `scripts/plot.py` Python script) present the comparative bandwidth performance between implementations for each routine:

![Comparative bandwidth performance for `memcmp_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/memcmp.png)

![Comparative bandwidth performance for `strchr_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strchr.png)

![Comparative bandwidth performance for `strcmp_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strcmp.png)

![Comparative bandwidth performance for `strcpy_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strcpy.png)

![Comparative bandwidth performance for `strlen_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strlen.png)

![Comparative bandwidth performance for `strncmp_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strncmp.png)

![Comparative bandwidth performance for `strnlen_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strnlen.png)

![Comparative bandwidth performance for `strrchr_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strrchr.png)
