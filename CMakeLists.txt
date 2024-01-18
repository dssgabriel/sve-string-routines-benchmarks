cmake_minimum_required(VERSION 3.16)
project(bench-string-aarch64-sve
    LANGUAGES C ASM
    DESCRIPTION "Comparative benchmarks for implementations of Arm SVE string routines"
    VERSION 0.1.0
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(bench-sve-string-routines
    src/bench.c
    src/driver.c
    src/stats.c
    src/utils.c
    src/main.c
    string-aarch64-sve/baseline/memcmp-sve.S
    string-aarch64-sve/baseline/strchr-sve.S
    string-aarch64-sve/baseline/strcmp-sve.S
    string-aarch64-sve/baseline/strcpy-sve.S
    string-aarch64-sve/baseline/strlen-sve.S
    string-aarch64-sve/baseline/strncmp-sve.S
    string-aarch64-sve/baseline/strnlen-sve.S
    string-aarch64-sve/baseline/strrchr-sve.S
    string-aarch64-sve/new/memcmp-sve.S
    string-aarch64-sve/new/strchr-sve.S
    string-aarch64-sve/new/strcmp-sve.S
    string-aarch64-sve/new/strcpy-sve.S
    string-aarch64-sve/new/strlen-sve.S
    string-aarch64-sve/new/strncmp-sve.S
    string-aarch64-sve/new/strnlen-sve.S
    string-aarch64-sve/new/strrchr-sve.S
)
target_include_directories(bench-sve-string-routines PRIVATE include)
target_compile_options(bench-sve-string-routines PRIVATE "-march=armv8.4-a+sve")
target_link_libraries(bench-sve-string-routines PRIVATE m)
