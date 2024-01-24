# Comparative benchmarks for SVE-optimized string routines


## About

This repository benchmarks the current implementations of some AArch64 SVE string routines from [Arm's optimized-routines library](https://github.com/ARM-software/optimized-routines) against newly proposed implementations.

This effort was carried out as part of the [EMOPASS]() European Project and conducted at the Laboratory of Parallelism and Distributed Algorithm Networks (LI-PaRAD) of the [University of Versailles Saint-Quentin-en-Yvelines](https://www.uvsq.fr/english) (UVSQ — Paris-Saclay University), in collaboration with industrial partners: [SiPearl](https://sipearl.com/en) and [Eviden](https://eviden.com/).

In the context of benchmarking the performance of HPC applications on Arm Neoverse V1 microarchitectures (in particular on the AWS Graviton3/Graviton3E and SiPearl Rhea processors), we have shown that using `INCx` SVE instructions to increment the loop offset can lead to noticeable performance degradation. Instead of using `INCx`, it is possible to replace it with a combination of `CNTx` to retrieve the SVE register's width in the loop prelude, and `ADD` to increment the offset in the loop (directly in the place of `INCx`). This change is only used in paths where the whole vector is known to be valid, i.e., no elements in the register are masked using a predicate. Conveniently, this also happens to be the critical path in most scenarios.

The benchmarks presented here show that this change _never_ causes any performance regression compared to the current implementations in the [ARM-software/optimized-routines](https://github.com/ARM-software/optimized-routines) repository.   
This optimization has been tested on AWS Graviton3 and Graviton3E instances and, depending on the routine and buffer size, achieves performance improvements up to 45%.   
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

Our approach to benchmarking tries to be as straightforward, accurate, and reproducible as possible on different systems. All benchmark results presented in this repository were obtained on a AWS Graviton3E CPU (`hpc7g.16xlarge` instance, on kernel `5.10.179-171.711.amzn2.aarch64`, and compiled with GCC 12.2.0 and/or ACFL 23.10).

We benchmarked each routine with the following (total) memory sizes:
- 32 KiB (half of the Graviton's 3 L1d cache)
- 64 KiB (full L1d cache)
- 512 KiB (half L2 cache)
- 1 MiB (full L2 cache)
- 16 MiB (half L3 cache)
- 32 MiB (full L3 cache)
- 256 MiB (8x L3 cache, guaranteed to be in DDR5 RAM)

These specific sizes allow us to showcase the routines' behavior in two main scenarios:
1. using half the cache size guarantees that the data should always fit in a given cache level;
2. using the full cache size means there is higher likelyhood that some of the data spills in a higher cache level (and thus, may reduce performance).

For routines that manipulate two strings (`memcmp`, `strcmp`, `strcpy` and `strncmp`), their length was divided by two to ensure that the sum of buffer sizes does not exceed the target memory footprint.
All strings are initialized with random ASCII characters, and a null terminator is inserted at the end to ensure that routines must process the entire buffer to run to completion.

Each measurement is sampled 31 times, which offers an accurate representation of runtime performance and stability.   
For smaller buffers, the execution time can be very short. In this case, the sampled execution time is obtained using the average runtime of an inner loop where the routine is called repeatedly.   
In order to ensure that data buffers are in cache before running the actual benchmark, a few iterations of warmup runs are performed.   
Peformance measurements are done using `clock_gettime` and the `CLOCK_MONOTONIC_RAW` clock, which provides a highly accurate resolution (up to the nanosecond scale).

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
- string buffer size
- minimum runtime (in ns)
- maximum runtime (in ns)
- median runtime (in ns)
- average (mean) runtime (in ns)
- percentage of runtime standard deviation
- average bandwidth (in GiB/s)
- bandwidth standard deviation (in GiB/s)
- speedup (as percentage of performance improvement)

The following is the raw result output for the `strcpy` routine:
```
  ROUTINE IMPLEMENTATION |  BUF SIZE | RT MIN ns | RT MED ns | RT MAX ns |    RT AVG ns | RT STDEV % | BW AVG GiB/s | BW STDEV GiB/s | SPEEDUP
-----------------------------------------------------------------------------------------------------------------------------------------------
    Current `strcpy_sve` |  16.0 KiB |      1327 |      1358 |      1400 |     1354.035 |      1.063 |       11.270 |          0.120 |
        New `strcpy_sve` |  16.0 KiB |      1071 |      1071 |      1084 |     1072.527 |      0.328 |       14.227 |          0.046 | +26.25%
-----------------------------------------------------------------------------------------------------------------------------------------------
    Current `strcpy_sve` |  32.0 KiB |      2710 |      2727 |      2772 |     2728.946 |      0.471 |       11.183 |          0.052 |
        New `strcpy_sve` |  32.0 KiB |      2136 |      2137 |      2185 |     2139.042 |      0.450 |       14.267 |          0.063 | +27.58%
-----------------------------------------------------------------------------------------------------------------------------------------------
    Current `strcpy_sve` | 256.0 KiB |     20621 |     20805 |     21661 |    20804.876 |      0.879 |       11.736 |          0.100 |
        New `strcpy_sve` | 256.0 KiB |     16727 |     16774 |     16996 |    16787.597 |      0.445 |       14.543 |          0.064 | +23.93%
-----------------------------------------------------------------------------------------------------------------------------------------------
    Current `strcpy_sve` | 512.0 KiB |     41227 |     41693 |     42581 |    41735.542 |      0.824 |       11.700 |          0.096 |
        New `strcpy_sve` | 512.0 KiB |     33644 |     33997 |     34646 |    34015.755 |      0.760 |       14.355 |          0.108 | +22.69%
-----------------------------------------------------------------------------------------------------------------------------------------------
    Current `strcpy_sve` |   8.0 MiB |    657770 |    668360 |    684544 |   668283.755 |      0.890 |       11.691 |          0.104 |
        New `strcpy_sve` |   8.0 MiB |    544223 |    566415 |    597360 |   565929.735 |      1.995 |       13.810 |          0.273 | +18.09%
-----------------------------------------------------------------------------------------------------------------------------------------------
    Current `strcpy_sve` |  16.0 MiB |   1317036 |   1335363 |   1369152 |  1337263.763 |      0.814 |       11.685 |          0.095 |
        New `strcpy_sve` |  16.0 MiB |   1081086 |   1150392 |   1192028 |  1142429.204 |      2.523 |       13.686 |          0.349 | +17.05%
-----------------------------------------------------------------------------------------------------------------------------------------------
    Current `strcpy_sve` | 128.0 MiB |  10765184 |  11029292 |  11234220 | 11006469.871 |      1.142 |       11.358 |          0.130 |
        New `strcpy_sve` | 128.0 MiB |   9578000 |   9932403 |  10316447 |  9959249.548 |      2.090 |       12.556 |          0.262 | +10.52%
```

Plots summarizing these results for each routine can be found in the  [`results/plots`](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/) directory, as well as at the end of this README.   
They can also be (re-)generated using the `script/plot_bw.py` Python script:
```sh
python scripts/plot_by.py --input results/raw/strcpy.dat --output results/plots/strcpy.png
```

### Observations

The optimization presented here _always_ yields better performance than the current implementation. Replacing `INCx` with `CNTx` and `ADD` is equivalent to hoisting the retrieval of the SVE register's width out of the loop, the only "penalty" being that it requires an additional register. This means that such an optimization should not produce slowdowns on other SVE-supporting microarchitectures (e.g., for embedded hardware or Neoverse V2) and should therefore be safe to merge.

The performance gap is generally higher for data that fits in the L1 or L2 cache, when the memory load's latency is small and exacerbates the slowdown incured by `INCx`. For larger strings, that fit in L3 or in RAM, fetching the data takes significantly more time, which partially hides the improvement obtained by replacing `INCx` instructions.

### Bandwidth plots

The following plots (generated with the `scripts/plot.py` Python script) present the comparative bandwidth performance between implementations for each routine, with the current implementation in blue, and our optimized version in red:
![Comparative bandwidth performance for `memcmp_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/memcmp.png)

![Comparative bandwidth performance for `strchr_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strchr.png)

![Comparative bandwidth performance for `strcmp_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strcmp.png)

![Comparative bandwidth performance for `strcpy_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strcpy.png)

![Comparative bandwidth performance for `strlen_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strlen.png)

![Comparative bandwidth performance for `strncmp_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strncmp.png)

![Comparative bandwidth performance for `strnlen_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strnlen.png)

![Comparative bandwidth performance for `strrchr_aarch64_sve` implementations](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/strrchr.png)
