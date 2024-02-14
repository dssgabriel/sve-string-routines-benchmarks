/**
 * Copyright © 2004 - 2024, Université de Versailles Saint-Quentin-en-Yvelines (UVSQ)
 * Copyright © 2024, Gabriel Dos Santos
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 **/

#pragma once

#include "types.h"

// Declarations for current string optimized-routines
extern int32_t __memcmp_aarch64_sve(void const* s1, void const* s2, size_t n);
extern void* __memcpy_aarch64_sve(void* restrict dst, void const* restrict src, size_t n);
extern int32_t __strcmp_aarch64_sve(char const* s1, char const* s2);
extern int32_t __strncmp_aarch64_sve(char const* dst, char const* src, size_t n);
extern char* __strchr_aarch64_sve(char const* s, int32_t c);
extern char* __strrchr_aarch64_sve(char const* s, int32_t c);
extern char* __strcpy_aarch64_sve(char* restrict dst, char const* restrict src);
extern size_t __strlen_aarch64_sve(char const* s);
extern size_t __strnlen_aarch64_sve(char const* s, size_t n);

// Declarations for the new implementations of string optimized-routines
extern int32_t new_memcmp_aarch64_sve(void const* s1, void const* s2, size_t n);
extern void* new_memcpy_aarch64_sve(void* restrict dst, void const* restrict src, size_t n);
extern int32_t new_strcmp_aarch64_sve(char const* s1, char const* s2);
extern int32_t new_strncmp_aarch64_sve(char const* dst, char const* src, size_t n);
extern char* new_strchr_aarch64_sve(char const* s, int32_t c);
extern char* new_strrchr_aarch64_sve(char const* s, int32_t c);
extern char* new_strcpy_aarch64_sve(char* restrict dst, char const* restrict src);
extern char* new_strncpy_aarch64_sve(char* restrict dst, char const* restrict src, size_t n);
extern size_t new_strlen_aarch64_sve(char const* s);
extern size_t new_strnlen_aarch64_sve(char const* s, size_t n);

// Function pointer type declarations
typedef int32_t memcmp_fn_t(void const*, void const*, size_t);
typedef void* memcpy_fn_t(void* restrict, void const* restrict, size_t);
typedef int32_t strcmp_fn_t(char const*, char const*);
typedef int32_t strncmp_fn_t(char const*, char const*, size_t);
typedef char* strchr_fn_t(char const*, int32_t);
typedef char* strrchr_fn_t(char const*, int32_t);
typedef char* strcpy_fn_t(char* restrict, char const* restrict);
typedef char* strncpy_fn_t(char* restrict, char const* restrict, size_t);
typedef size_t strlen_fn_t(char const*);
typedef size_t strnlen_fn_t(char const*, size_t);

void driver_memcmp(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    memcmp_fn_t* memcmp_fn,
    void const* s1,
    void const* s2,
    size_t n
);

void driver_memcpy(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    memcpy_fn_t* memcpy_fn,
    void* restrict dst,
    void const* restrict src,
    size_t n
);

void driver_strcmp(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strcmp_fn_t* strcmp_fn,
    char const* s1,
    char const* s2
);

void driver_strncmp(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strncmp_fn_t* strncmp_fn,
    char const* s1,
    char const* s2,
    size_t n
);

void driver_strchr(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strchr_fn_t* strchr_fn,
    char const* s,
    int32_t c
);

void driver_strrchr(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strrchr_fn_t* strrchr_fn,
    char const* s,
    int32_t c
);

void driver_strcpy(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strcpy_fn_t* strcpy_fn,
    char* dst,
    char const* src
);

void driver_strncpy(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strncpy_fn_t* strncpy_fn,
    char* dst,
    char const* src,
    size_t n
);

void driver_strlen(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strlen_fn_t* strlen_fn,
    char const* s
);

void driver_strnlen(
    size_t nsamples,
    size_t nreps,
    double samples[nsamples],
    strnlen_fn_t* strnlen_fn,
    char const* s,
    size_t n
);
