# Comparative benchmarks for SVE-optimized string routines


## About

This repository benchmarks the current implementations of some AArch64 SVE string routines from [Arm's optimized-routines library](https://github.com/ARM-software/optimized-routines) against newly proposed implementations.

This effort was carried out as part of the EMOPASS French Project and conducted at the Laboratory of Parallel Networks and Distributed Algorithms (LI-PaRAD) of the [University of Versailles Saint-Quentin-en-Yvelines](https://www.uvsq.fr/english) (UVSQ — Paris-Saclay University), in collaboration with industrial partners: [SiPearl](https://sipearl.com/en) and [Eviden](https://eviden.com/).

In the context of benchmarking HPC applications on Arm Neoverse V1 microarchitectures (in particular the AWS Graviton3 and Graviton3E processors), we have shown that using `INCx` SVE instructions to increment the loop offset can introduce significant performance degradation. Instead, it is possible to replace this instruction with a combination of `CNTx` to retrieve the SVE register width in the loop prelude, and `ADD` to increment the loop offset accordingly. This change is only used in paths where the whole vector is known to be valid, i.e., when no elements in the register are masked through a predicate. Conveniently, this happens to be the critical path in most scenarios.

The benchmarks presented here show that this change _never_ causes any performance regression compared to the current implementations in the [ARM-software/optimized-routines](https://github.com/ARM-software/optimized-routines) repository. Moreover, some of the proposed implementations are competitive with their GNU libc counterparts, namely: `strcpy`, `strncpy`, `strcmp` and `strncmp`.
Tests have been conducted on AWS Graviton3 and Graviton3E instances. Detailed results are presented in the [Results](#results) section at the end of this README.


## Build & run

### Pre-requisites

- Arm SVE enabled CPU
- CMake 3.16+
- C11 conforming compiler

### Building

```sh
cmake -S . -B build
cmake --build build
```

By default, this will compile the code to compare against AOR implementations. You can enable comparison against GNU libc by passing `-DCMP_LIBC` to the `CMAKE_C_FLAGS` variable.

### Running

All program options can be listed with the following command:
```sh
./build/bench-sve-string-routines --help
```

Run specific routines with their name as option. E.g. for `strncmp` and `strcpy`:
```sh
./build/bench-sve-string-routines --strncmp --strcpy
```


## Results

Please check out the [dedicated README](https://github.com/dssgabriel/sve-string-routines-benchmarks/tree/main/results) in the `results/` directory.
