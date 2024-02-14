/*
 * strcpy/stpcpy - copy a string returning pointer to start/end.
 *
 * Copyright (c) 2018-2022, Arm Limited.
 * SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception
 */

/* Assumptions:
 *
 * ARMv8-a, AArch64, SVE available.
 */

#include "../asmdefs.h"

#ifdef HAVE_SVE

.arch armv8-a+sve

/* To build as stpcpy, define BUILD_STPCPY before compiling this file.  */
#ifdef BUILD_STPCPY
#define FUNC new_stpcpy_aarch64_sve
#else
#define FUNC new_strcpy_aarch64_sve
#endif

ENTRY (FUNC)
	PTR_ARG (0)
	PTR_ARG (1)

	ptrue	p0.b, all		/* all 1s, loop invariant */
	mov	x2, xzr			/* initialize offset */
	cntb	x3			/* initialize stride */

	.p2align 4
	/* Read a vector's worth of bytes. */
L(loop):
	ld1b	z0.b, p0/z, [x1, x2]
	cmpeq	p1.b, p0/z, z0.b, #0	/* search for zeros */
	b.any	L(zero)

	/* No zero found.  Store the whole vector and loop. */
	st1b	z0.b, p0, [x0, x2]
	add	x2, x2, x3
	b	L(loop)

	/* Zero found.  Crop the vector to the found zero and finish. */
L(zero):
	brka	p0.b, p0/z, p1.b
	st1b	z0.b, p0, [x0, x2]
#ifdef BUILD_STPCPY
	add	x0, x0, x2
	sub	x0, x0, #1
	incp	x0, p2.b
#endif
L(return):
	ret

END (FUNC)

#endif
