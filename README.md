# Comparative benchmarks for SVE-optimized string routines


## About

This repository benchmarks the current implementations of some AArch64 SVE string routines from [Arm's optimized-routines library](https://github.com/ARM-software/optimized-routines) against newly proposed implementations.

This effort was carried out as part of the EMOPASS French Project and conducted at the Laboratory of Parallelism and Distributed Algorithm Networks (LI-PaRAD) of the [University of Versailles Saint-Quentin-en-Yvelines](https://www.uvsq.fr/english) (UVSQ — Paris-Saclay University), in collaboration with industrial partners: [SiPearl](https://sipearl.com/en) and [Eviden](https://eviden.com/).

In the context of benchmarking the performance of HPC applications on Arm Neoverse V1 microarchitectures (in particular on the AWS Graviton3 and Graviton3E processors), we have shown that using `INCx` SVE instructions to increment the loop offset can lead to noticeable performance degradation. Instead of using `INCx`, it is possible to replace it with a combination of `CNTx` to retrieve the SVE register's width in the loop prelude, and `ADD` to increment the offset in the loop (directly in the place of `INCx`). This change is only used in paths where the whole vector is known to be valid, i.e., when no elements in the register are masked using a predicate. Conveniently, this also happens to be the critical path in most scenarios.

The benchmarks presented here show that this change _never_ causes any performance regression compared to the current implementations in the [ARM-software/optimized-routines](https://github.com/ARM-software/optimized-routines) repository.   
This optimization has been tested on AWS Graviton3 and Graviton3E instances. Depending on the routine and buffer size, it achieves performance improvements of over 45%.   
Test string sizes range from 8 B (half the size of the Graviton3's L1 data cache) to 256 MiB (8x the size of its L3 cache). Detailed results are presented in the [Results](#results) section at the end of this README.


## Build & run

### Pre-requisites

- CPU supporting the Arm SVE ISA extension
- CMake 3.16
- C11-compatible compiler

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

## Results

Please the dedicated README in the [results/](https://github.com/dssgabriel/sve-string-routines-benchmarks/tree/main/results) directory.
