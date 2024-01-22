# Comparative benchmarks for SVE-optimized string routines


## About

This repository benchmarks the current implementations of some AArch64 SVE string routines from [Arm's optimized-routines library](https://github.com/ARM-software/optimized-routines) against newly proposed implementations.

This project was done as part of the EMOPASS European Project and conducted at the University of Versailles — Saint-Quentin-en-Yvelines (UVSQ) by the LI-PaRAD computer science laboratory, and in collaboration with industrial partners: SiPearl and Atos/Eviden.

In the context of benchmarking the performance of HPC applications on Arm Neoverse V1 microarchitectures (in particular on the AWS Graviton3 and SiPearl Rhea1 processors), we were able to show that using `INCx` SVE instructions to increment the loop counter can lead to noticeable performance degradation. Instead, it is possible to replace `INCx` by using a combination of `CNTx` to retrieve the SVE register's width in the loop prelude, and `ADD` to increment the offset in the loop (directly in place of `INCx`). This change can only be used reliably in paths where the whole vector is known to be valid, i.e., no elements in the register are masked. Conveniently, this also happens to be the critical path in most scenarios.

The benchmarks presented here show that this change _never_ causes any performance regression compared to the current implementations in the [ARM-software/optimized-routines](https://github.com/ARM-software/optimized-routines)) repository.   
This optimization has been tested on a AWS Graviton3 instance and, depending on the routine and buffer size, leads to performance improvements ranging from ~2% to ~46%.   
Test string sizes range from 32 KiB (half the size of the Graviton3's L1 data cache), to 256 MiB (8x the size of its L3 cache). Detailed results are presented in the [Results](#results) section at the end of this README.


## Build & run

### Pre-requisites

- CPU supporting the Arm SVE ISA extension
- CMake 3.16
- C11-conformant C compiler

### Building

```sh
cmake -S . -B build
cmake --build build
```

### Running

All program options can be listed with the following command:
```sh
./build/bench-sve-string-routines --help
```

Run specific routines with their name as option, e.g., for `memcmp` and `strcpy`:
```sh
./build/bench-sve-string-routines --memcmp --strcpy
```


## Benchmark methodology

Our approach to benchmarking tries to be as straightforward, accurate, and reproducible as possible on different systems. All benchmark results presented in this repository were donce on a AWS Graviton3 CPU (`c7g.medium` instance, on kernel `5.15.0-1051-aws` and using GCC 11.1.0).

For all routines, we used the following using test buffer sizes, initialized with random ASCII characters:
- 32 KiB, half of the Graviton's 3 L1d$
- 64 KiB, full L1d$
- 512 KiB, half L2$
- 1 MiB, full L2$
- 16 MiB, half L3$
- 32 MiB, full L3$
- 256 MiB, 8x L3$, guaranteed to be in DDR5 RAM

Each measurement is sampled 31 times, which guarantees an accurate representation of runtime performance and stability.   
For small buffers, the execution time can be very short (less than 1 µs). In this case, the sampled execution time is obtained using the averaged execution time of an inner loop where the routine is called repeatedly.   
In order to ensure that data buffers are in cache before running the actual benchmark, a few iterations of warmup runs are performed.   
Peformance measurements are done using `clock_gettime` and the `CLOCK_MONOTONIC_RAW` clock, which offers the most accurate resolution (up to a few nanoseconds).

The general benchmarking algorithm is:
```
INPUTS:
  routine:           The routine implementation to benchmark.
  warmup_count:      The number of warmup runs.
  sample_count:      The number of sample measurements.
  repetitions_count: The number of calls to average on per samples.
──────────────────────────────────────────────────────────────────────────────────────────
OUTPUT:
  A list of statistical metrics for a given routine/buffer size combination.
──────────────────────────────────────────────────────────────────────────────────────────
VARIABLES:
  start:     An instant in time which marks the start of the benchmarked code section.
  stop:      An instant in time which marks the end of the benchmarked code section.
  exec_time: The execution time for a given routine/buffer size combination.
  samples:   A list of execution times for a given routine/buffer size
             combination.
──────────────────────────────────────────────────────────────────────────────────────────
PROCEDURE benchmark_driver:
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

    RETURN bench_process(samples)
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
