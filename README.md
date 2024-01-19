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

The general algorithm is:
```
for w in 0 to WARMUP_COUNT:
    call(routine)
end

for s in 0 to SAMPLE_COUNT:
    start = clock_gettime()
    for r in 0 to REPETITIONS_COUNT:
        call(routine)
    end
    stop = clock_gettime()
    sample[s] = (stop - start) / REPETITIONS_COUNT
end
```

## Results

**TLDR: Our modified implementation of the string routines systematically yields better performance across all buffer sizes.**

### Result format

The raw results are available in the [`results/raw`](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/raw/) directory. These include the following metrics for each benchmarked routine:
- string buffer size (in Bytes)
- minimum execution time (in ns)
- maximum execution time (in ns)
- median execution time (in ns)
- average (mean) execution time (in ns)
- percentage of standard deviation (i.e., runtime stability of the routine)
- bandwidth (in GiB/s)
- speedup (as percentage of performance improvement)

The following is an example raw output for the `strcpy` routine:
```
      ROUTINE IMPLEM |    BUF SIZE |     MINIMUM ns |      MEDIAN ns |     MAXIMUM ns |     AVERAGE ns |   STD DEV % |    BW GiB/s |   SPEEDUP %
--------------------------------------------------------------------------------------------------------------------------------------------------
Current `strcpy_sve` |    32.0 KiB |           2721 |           2734 |           2741 |       2731.415 |       0.217 |      11.173 |
    New `strcpy_sve` |    32.0 KiB |           2140 |           2141 |           2145 |       2141.367 |       0.053 |      14.251 |    +27.55%
--------------------------------------------------------------------------------------------------------------------------------------------------
Current `strcpy_sve` |    64.0 KiB |           5469 |           5506 |           5534 |       5503.085 |       0.237 |      11.091 |
    New `strcpy_sve` |    64.0 KiB |           4271 |           4273 |           4280 |       4273.643 |       0.056 |      14.282 |    +28.77%
--------------------------------------------------------------------------------------------------------------------------------------------------
Current `strcpy_sve` |   512.0 KiB |          41216 |          41592 |          42391 |      41627.693 |       0.656 |      11.730 |
    New `strcpy_sve` |   512.0 KiB |          33617 |          33819 |          35242 |      33900.303 |       1.000 |      14.403 |    +22.79%
--------------------------------------------------------------------------------------------------------------------------------------------------
Current `strcpy_sve` |     1.0 MiB |          83185 |          85698 |          87653 |      85604.782 |       1.083 |      11.408 |
    New `strcpy_sve` |     1.0 MiB |          69769 |          71591 |          73799 |      71772.921 |       1.447 |      13.606 |    +19.27%
--------------------------------------------------------------------------------------------------------------------------------------------------
Current `strcpy_sve` |    16.0 MiB |        1326971 |        1353345 |        1568080 |    1356537.926 |       2.969 |      11.518 |
    New `strcpy_sve` |    16.0 MiB |        1099110 |        1141193 |        1165957 |    1138417.229 |       1.217 |      13.725 |    +19.16%
--------------------------------------------------------------------------------------------------------------------------------------------------
Current `strcpy_sve` |    32.0 MiB |        2664317 |        2709415 |        4207792 |    2759614.116 |       9.778 |      11.324 |
    New `strcpy_sve` |    32.0 MiB |        2245466 |        2321750 |        2372626 |    2318563.877 |       1.466 |      13.478 |    +19.02%
--------------------------------------------------------------------------------------------------------------------------------------------------
Current `strcpy_sve` |   256.0 MiB |       21878516 |       22283954 |       78539024 |   24056714.548 |      42.039 |      10.392 |
    New `strcpy_sve` |   256.0 MiB |       20017984 |       20320203 |       20786768 |   20341268.968 |       1.038 |      12.290 |    +18.27%
--------------------------------------------------------------------------------------------------------------------------------------------------
```

### Observations

The optimization presented here yields better performance than the current implementation in all scenarios. This is because replacing `INCx` with `CNTx` and `ADD` is equivalent to hoisting the retrieval of the SVE register's width out of the loop, the only "penalty" being that it requires an additional register.   
The performance gap is generally higher for data that fits in the L1 or L2 cache, when the memory load's latency is small and highlights the penalty incured by `INCx`. For larger strings that fit in L3 or in RAM, fetching the data takes significantly more time, which partially hides the improvement obtained by replacing `INCx` instructions.

Performance improvements ranges:
- `memcmp`: 12.82% to 46.27% faster
- `strchr`: 16.97% to 36.85% faster
- `strcmp`: 10.54% to 33.45% faster
- `strcpy`: 9.13% to 28.62% faster
- `strlen`: 5.86% to 17.86% faster
- `strncmp`: 1.98% to 10.12% faster
- `strnlen`: 10.67% to 19.27% faster
- `strrchr`: 33.52% to 38.12% faster

The following plots (generated with the `scripts/plot.py` Python script) present the comparative bandwidth performance between implementations for each routine, with the current implementation in blue, and our optimized version in red:
![Comparative bandwidth performance for `memcmp_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/memcmp.png)

![Comparative bandwidth performance for `strchr_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strchr.png)

![Comparative bandwidth performance for `strcmp_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strcmp.png)

![Comparative bandwidth performance for `strcpy_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strcpy.png)

![Comparative bandwidth performance for `strlen_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strlen.png)

![Comparative bandwidth performance for `strncmp_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strncmp.png)

![Comparative bandwidth performance for `strnlen_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strnlen.png)

![Comparative bandwidth performance for `strrchr_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strrchr.png)
