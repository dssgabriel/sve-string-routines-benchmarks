# Copyright © 2004 - 2024, Université de Versailles Saint-Quentin-en-Yvelines (UVSQ)
# Copyright © 2024, Gabriel Dos Santos
#
# This is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
# USA.

cmake_minimum_required(VERSION 3.16)
project(bench-string-aarch64-sve
    LANGUAGES C ASM
    DESCRIPTION "Comparative benchmarks for alternate implementations of Arm SVE-optimized string routines"
    VERSION 0.4.1
)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)

if(CMAKE_BUILD_TYPE EQUAL Debug)
    add_compile_options(-Wall -Wextra -pedantic -Wconversion -DDEBUG)
endif()

add_executable(bench-sve-string-routines
    src/bench.c
    src/driver.c
    src/stats.c
    src/utils.c
    src/main.c
    string/aarch64/baseline/memcmp-sve.S
    string/aarch64/baseline/memcpy-sve.S
    string/aarch64/baseline/strcmp-sve.S
    string/aarch64/baseline/strncmp-sve.S
    string/aarch64/baseline/strchr-sve.S
    string/aarch64/baseline/strrchr-sve.S
    string/aarch64/baseline/strcpy-sve.S
    string/aarch64/baseline/strlen-sve.S
    string/aarch64/baseline/strnlen-sve.S
    string/aarch64/new/memcmp-sve.S
    string/aarch64/new/memcpy-sve.S
    string/aarch64/new/strcmp-sve.S
    string/aarch64/new/strncmp-sve.S
    string/aarch64/new/strchr-sve.S
    string/aarch64/new/strrchr-sve.S
    string/aarch64/new/strcpy-sve.S
    string/aarch64/new/strncpy-sve.S
    string/aarch64/new/strlen-sve.S
    string/aarch64/new/strnlen-sve.S
)
target_include_directories(bench-sve-string-routines PUBLIC include)
target_compile_options(bench-sve-string-routines PUBLIC "-march=armv8.4-a+sve")
target_link_libraries(bench-sve-string-routines PUBLIC m)
