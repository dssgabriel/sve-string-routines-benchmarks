# Results

## Benchmark methodology

Our approach to benchmarking tries to be as straightforward, accurate, and reproducible as possible on different systems. All benchmark results presented in this repository were obtained on an AWS Graviton3E CPU (`hpc7g.16xlarge` instance, on kernel `6.2.0-1018-aws`, compiled with GCC 12.3.0).

We benchmark each routine using two sets of sizes:
1. Short strings: ranging from 1 to 128 Bytes (with increments of 1), `-DSHORT_STRS`;
2. Full size: ranging from 128 Bytes to 512 MiB (doubling the size each time), `-DFULL_SIZE_RANGE`.

We can also enable aligned data allocations using the `-DALIGNED_ALLOCS` compile-time flag.

All strings are initialized with random alphanumeric characters (non-null ASCII for `memcmp` and `memcpy`), and a null terminator is inserted at the end to ensure that routines must process the entire buffer to run to completion.

Each measurement is sampled 101 times, which offers an accurate representation of runtime performance and stability.   
For smaller buffers, the execution time can be very short. In this case, the sampled execution time is obtained using the average runtime of an inner loop where the routine is called repeatedly.   
In order to ensure that data buffers are in cache before running the actual benchmark, a few iterations of warmup runs are performed.   
Performance measurements are done using `clock_gettime` and the `CLOCK_MONOTONIC_RAW` clock, which provides a highly accurate resolution (up to the nanosecond scale).

The general benchmarking algorithm is:
```
INPUTS:
  routine:           The routine implementation to benchmark.
  warmup_count:      The number of warmup runs.
  sample_count:      The number of sample measurements.
  repetitions_count: The number of calls to repeat per sample.
──────────────────────────────────────────────────────────────────────────────────────────
OUTPUT:
  A list of statistical metrics for a given routine/buffer size combination.
──────────────────────────────────────────────────────────────────────────────────────────
VARIABLES:
  start:     An instant in time that marks the start of the benchmarked code section.
  stop:      An instant in time that marks the end of the benchmarked code section.
  exec_time: The execution time for a given routine/buffer size combination.
  samples:   A list of execution times for a given routine/buffer size combination.
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

## Result format

The raw results are available in the [`results/raw`](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/raw/) directory. These include the following metrics for each benchmarked routine:
- string buffer size (in B)
- minimum runtime (in ns)
- maximum runtime (in ns)
- median runtime (in ns)
- average (mean) runtime (in ns)
- percentage of runtime standard deviation
- average bandwidth (in GiB/s)
- bandwidth standard deviation (in GiB/s)
- speedup (as a percentage of performance improvement)

The following is the (truncated) raw output for the `strcpy` routine:
```
        ROUTINE IMPLEMENTATION |  BUF SIZE B |      RT MIN ns |      RT MED ns |      RT MAX ns |      RT AVG ns |     RT STDEV % |   BW AVG GiB/s | BW STDEV GiB/s |     SPEEDUP
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        strcpy (GNU libc 2.39) |           1 |          2.050 |          2.061 |          2.079 |          2.061 |          0.224 |          0.452 |          0.001 |
             strcpy (LI-PaRAD) |           1 |          2.310 |          2.311 |          2.315 |          2.311 |          0.059 |          0.403 |          0.000 |     -10.84%
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        strcpy (GNU libc 2.39) |           2 |          3.079 |          3.082 |          3.086 |          3.082 |          0.055 |          0.604 |          0.000 |
             strcpy (LI-PaRAD) |           2 |          1.923 |          2.310 |          2.319 |          2.208 |          7.736 |          0.849 |          0.071 |     +39.57%
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        strcpy (GNU libc 2.39) |           3 |          3.079 |          3.082 |          3.094 |          3.082 |          0.075 |          0.907 |          0.001 |
             strcpy (LI-PaRAD) |           3 |          2.310 |          2.311 |          2.315 |          2.311 |          0.062 |          1.209 |          0.001 |     +33.35%
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        strcpy (GNU libc 2.39) |           4 |          2.695 |          2.700 |          3.085 |          2.867 |          6.668 |          1.305 |          0.086 |
             strcpy (LI-PaRAD) |           4 |          1.923 |          1.930 |          2.315 |          2.113 |          9.126 |          1.778 |          0.161 |     +35.69%
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        strcpy (GNU libc 2.39) |           5 |          2.695 |          2.696 |          2.700 |          2.697 |          0.062 |          1.727 |          0.001 |
             strcpy (LI-PaRAD) |           5 |          2.310 |          2.311 |          2.315 |          2.311 |          0.060 |          2.015 |          0.001 |     +16.67%
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        strcpy (GNU libc 2.39) |           6 |          2.695 |          2.697 |          2.701 |          2.697 |          0.064 |          2.072 |          0.001 |
             strcpy (LI-PaRAD) |           6 |          1.923 |          2.310 |          2.315 |          2.126 |          9.061 |          2.651 |          0.242 |     +26.87%
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        strcpy (GNU libc 2.39) |           7 |          2.695 |          2.696 |          2.709 |          2.697 |          0.079 |          2.417 |          0.002 |
             strcpy (LI-PaRAD) |           7 |          2.310 |          2.311 |          2.315 |          2.311 |          0.058 |          2.821 |          0.002 |     +16.68%
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        strcpy (GNU libc 2.39) |           8 |          2.695 |          2.697 |          2.701 |          2.697 |          0.067 |          2.763 |          0.002 |
             strcpy (LI-PaRAD) |           8 |          2.310 |          2.311 |          2.333 |          2.312 |          0.114 |          3.223 |          0.004 |     +16.65%
```

Plots summarizing these results for each routine can be found in the  [`results/plots`](https://github.com/dssgabriel/sve-string-routines-benchmarks/blob/main/results/plots/) directory, as well as at the end of this README.    
They can also be (re-)generated using the `script/plot_bw.py` Python script:
```sh
python scripts/plot_bw.py --routine $ROUTINE --input results/raw/$ROUTINE.dat --output results/plots/$ROUTINE.png 
```

## Optimization observations

Replacing `INCx` with `CNTx` and `ADD` is equivalent to hoisting the retrieval of the SVE register's width out of the loop, the only "penalty" being that it requires an additional register. This means that such optimization should not produce slowdowns on other SVE-supporting microarchitectures (e.g., for embedded hardware, A64FX, Neoverse V2, etc...) and should therefore be safe to merge.

The performance gap is generally higher for data that fits in the L1 or L2 cache when the memory load's latency is small and exacerbates the slowdown incurred by `INCx`. For larger strings, that fit in L3 or in RAM, fetching the data takes significantly more time, which partially hides the improvement obtained by replacing `INCx` instructions.

Compared to Arm's optimized-routines implementations, we've removed any dependencies to FFR-related instructions, which significantly reduces execution time, and also more closely matches the current implementations in GNU libc.

## Bandwidth plots

The following plots present the comparative bandwidth performance between implementations for each routine, with the current implementation in blue, and our optimized version in yellow:

### `strcpy`

Our implementation of `strcpy` is significantly faster than the latest (at the time of writing) GNU libc, especially on larger strings.   
On short strings, some specific string lengths show noticeable performance drops when the data is not aligned. This behavior is strange and would require further investigation, ideally on different hardware than AWS Graviton3/3E.

However, we think this is worth merging into AOR/libc as the overall performance gains largely outweigh the losses shown on some short string lengths with unaligned data. 

<p>
  <img src="plots/short_strs/noalign/strcpy_libc.png" width=49% alt="Average bandwidth performance of `strcpy` on short strings (unaligned data)">
  <img src="plots/short_strs/align/strcpy_libc.png" align="right" width=49% alt="Average bandwidth performance of `strcpy` on short strings (aligned data)">
</p>
<p>
  <img src="plots/full_sizes/noalign/strcpy_libc.png" width=49% alt="Average bandwidth performance of `strcpy` (unaligned data)">
  <img src="plots/full_sizes/align/strcpy_libc.png" align="right" width=49% alt="Average bandwidth performance of `strcpy` (aligned data)">
</p>

---

### `strncpy`

Our implementation of `strncpy` largely beats GNU libc in almost all scenarios. Just like `strcpy`, some sizes are problematic for short, unaligned strings. However, this isn't as problematic as the SVE implementation matches the performance of the glibc, even on the worst-case string lengths. On larger data, we easily outperform the current implementation.

<p>
  <img src="plots/short_strs/noalign/strncpy_libc.png" width=49% alt="Average bandwidth performance of `strncpy` on short strings (unaligned data)">
  <img src="plots/short_strs/align/strncpy_libc.png" align="right" width=49% alt="Average bandwidth performance of `strncpy` on short strings (aligned data)">
</p>
<p>
  <img src="plots/full_sizes/noalign/strncpy_libc.png" width=49% alt="Average bandwidth performance of `strncpy` (unaligned data)">
  <img src="plots/full_sizes/align/strncpy_libc.png" align="right" width=49% alt="Average bandwidth performance of `strncpy` (aligned data)">
</p>

---

### `strcmp`

For routines that don't need to write data to memory (i.e., no store instructions required), such as `strcmp`, we do not experience any slowdowns, even on unaligned data. Our implementation is thus better than the current GNU libc in all cases, except for string sizes in the ranges 1-16 bytes and 32-48 bytes, where we remain marginally slower than the NEON implementation from libc.

<p>
  <img src="plots/short_strs/noalign/strcmp_libc.png" width=49% alt="Average bandwidth performance of `strcmp` on short strings (unaligned data)">
  <img src="plots/short_strs/align/strcmp_libc.png" align="right" width=49% alt="Average bandwidth performance of `strcmp` on short strings (aligned data)">
</p>
<p>
  <img src="plots/full_sizes/noalign/strcmp_libc.png" width=49% alt="Average bandwidth performance of `strcmp` (unaligned data)">
  <img src="plots/full_sizes/align/strcmp_libc.png" align="right" width=49% alt="Average bandwidth performance of `strcmp` (aligned data)">
</p>

---

### `strncmp`

Much like `strcmp`, our implementation of `strncmp` always performs better than glibc, except for string lengths in the range of 1-16 bytes, where we are very slightly behind.

<p>
  <img src="plots/short_strs/noalign/strncmp_libc.png" width=49% alt="Average bandwidth performance of `strncmp` on short strings (unaligned data)">
  <img src="plots/short_strs/align/strncmp_libc.png" align="right" width=49% alt="Average bandwidth performance of `strncmp` on short strings (aligned data)">
</p>
<p>
  <img src="plots/full_sizes/noalign/strncmp_libc.png" width=49% alt="Average bandwidth performance of `strncmp` (unaligned data)">
  <img src="plots/full_sizes/align/strncmp_libc.png" align="right" width=49% alt="Average bandwidth performance of `strncmp` (aligned data)">


## Inconclusive results

We include hereafter the bandwidth plots for other string routine implementations that yielded inconclusive results.

<details>
<summary>Click to expand</summary>

### `memcpy`

<p>
  <img src="plots/short_strs/noalign/memcpy_libc.png" width=49% alt="Average bandwidth performance of `memcpy` on short strings (unaligned data)">
  <img align="right" src="plots/short_strs/align/memcpy_libc.png" width=49% alt="Average bandwidth performance of `memcpy` on short strings (aligned data)">
</p>
<p>
  <img src="plots/full_sizes/noalign/memcpy_libc.png" width=49% alt="Average bandwidth performance of `memcpy` (unaligned data)">
  <img align="right" src="plots/full_sizes/align/memcpy_libc.png" width=49% alt="Average bandwidth performance of `memcpy` (aligned data)">
</p>

---

### `memcmp`

<p>
  <img src="plots/short_strs/noalign/memcmp_libc.png" width=49% alt="Average bandwidth performance of `memcmp` on short strings (unaligned data)">
  <img align="right" src="plots/short_strs/align/memcmp_libc.png" width=49% alt="Average bandwidth performance of `memcmp` on short strings (aligned data)">
</p>
<p>
  <img src="plots/full_sizes/noalign/memcmp_libc.png" width=49% alt="Average bandwidth performance of `memcmp` (unaligned data)">
  <img align="right" src="plots/full_sizes/align/memcmp_libc.png" width=49% alt="Average bandwidth performance of `memcmp` (aligned data)">
</p>

---

### `strlen`

<p>
  <img src="plots/short_strs/noalign/strlen_libc.png" width=49% alt="Average bandwidth performance of `strlen` on short strings (unaligned data)">
  <img align="right" src="plots/short_strs/align/strlen_libc.png" width=49% alt="Average bandwidth performance of `strlen` on short strings (aligned data)">
</p>
<p>
  <img src="plots/full_sizes/noalign/strlen_libc.png" width=49% alt="Average bandwidth performance of `strlen` (unaligned data)">
  <img align="right" src="plots/full_sizes/align/strlen_libc.png" width=49% alt="Average bandwidth performance of `strlen` (aligned data)">
</p>

---

### `strnlen`

<p>
  <img src="plots/short_strs/noalign/strnlen_libc.png" width=49% alt="Average bandwidth performance of `strnlen` on short strings (unaligned data)">
  <img align="right" src="plots/short_strs/align/strnlen_libc.png" width=49% alt="Average bandwidth performance of `strnlen` on short strings (aligned data)">
</p>
<p>
  <img src="plots/full_sizes/noalign/strnlen_libc.png" width=49% alt="Average bandwidth performance of `strnlen` (unaligned data)">
  <img align="right" src="plots/full_sizes/align/strnlen_libc.png" width=49% alt="Average bandwidth performance of `strnlen` (aligned data)">
</p>

---

### `strchr`

<p>
  <img src="plots/short_strs/noalign/strchr_libc.png" width=49% alt="Average bandwidth performance of `strchr` on short strings (unaligned data)">
  <img align="right" src="plots/short_strs/align/strchr_libc.png" width=49% alt="Average bandwidth performance of `strchr` on short strings (aligned data)">
</p>
<p>
  <img src="plots/full_sizes/noalign/strchr_libc.png" width=49% alt="Average bandwidth performance of `strchr` (unaligned data)">
  <img align="right" src="plots/full_sizes/align/strchr_libc.png" width=49% alt="Average bandwidth performance of `strchr` (aligned data)">
</p>

---

### `strrchr`

<p>
  <img src="plots/short_strs/noalign/strrchr_libc.png" width=49% alt="Average bandwidth performance of `strrchr` on short strings (unaligned data)">
  <img align="right" src="plots/short_strs/align/strrchr_libc.png" width=49% alt="Average bandwidth performance of `strrchr` on short strings (aligned data)">
</p>
<p>
  <img src="plots/full_sizes/noalign/strrchr_libc.png" width=49% alt="Average bandwidth performance of `strrchr` (unaligned data)">
  <img align="right" src="plots/full_sizes/align/strrchr_libc.png" width=49% alt="Average bandwidth performance of `strrchr` (aligned data)">
</p>

</details>
