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

The general benchmarking algorithm is:
```
FOR EACH w IN [0, warmup_count]:
    CALL(routine)
END

FOR EACH s IN [0, sample_count]:
    start <- clock_gettime()
    FOR EACH r IN [0, repetitions_count]:
        CALL(routine)
    END
    stop <- clock_gettime()
    exec_time <- (stop - start) / repetitions_count
    sample[s] <- exec_time
END
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
-----------------------------------------------------------------------------------------------------------------------------------------------------
    Current `strcpy_sve` |    32.0 KiB |           2713 |           2740 |           2796 |       2738.157 |       0.523 |      11.145 |
        New `strcpy_sve` |    32.0 KiB |           2139 |           2139 |           2148 |       2140.814 |       0.182 |      14.255 |     +27.90%
-----------------------------------------------------------------------------------------------------------------------------------------------------
    Current `strcpy_sve` |    64.0 KiB |           5450 |           5514 |           5553 |       5509.550 |       0.503 |      11.078 |
        New `strcpy_sve` |    64.0 KiB |           4266 |           4267 |           4706 |       4283.721 |       1.853 |      14.248 |     +28.62%
-----------------------------------------------------------------------------------------------------------------------------------------------------
    Current `strcpy_sve` |   512.0 KiB |          42137 |          42494 |          44082 |      42536.994 |       0.864 |      11.479 |
        New `strcpy_sve` |   512.0 KiB |          34800 |          36138 |          37829 |      36084.024 |       2.152 |      13.532 |     +17.88%
-----------------------------------------------------------------------------------------------------------------------------------------------------
    Current `strcpy_sve` |     1.0 MiB |          83201 |          83917 |          87320 |      84265.287 |       1.339 |      11.589 |
        New `strcpy_sve` |     1.0 MiB |          68942 |          69856 |          73293 |      70257.758 |       1.752 |      13.900 |     +19.94%
-----------------------------------------------------------------------------------------------------------------------------------------------------
    Current `strcpy_sve` |    16.0 MiB |        1329583 |        1343019 |        1388090 |    1346961.458 |       1.091 |      11.600 |
        New `strcpy_sve` |    16.0 MiB |        1099795 |        1135635 |        1184362 |    1135509.387 |       1.867 |      13.760 |     +18.62%
-----------------------------------------------------------------------------------------------------------------------------------------------------
    Current `strcpy_sve` |    32.0 MiB |        2622362 |        2698347 |        2843760 |    2715358.774 |       2.241 |      11.509 |
        New `strcpy_sve` |    32.0 MiB |        2149314 |        2261621 |        2554164 |    2284286.161 |       5.146 |      13.680 |     +18.87%
-----------------------------------------------------------------------------------------------------------------------------------------------------
    Current `strcpy_sve` |   256.0 MiB |       21618563 |       22149558 |       22772514 |   22156929.032 |       1.310 |      11.283 |
        New `strcpy_sve` |   256.0 MiB |       19919944 |       20246309 |       20945148 |   20303257.065 |       1.264 |      12.313 |      +9.13%
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
