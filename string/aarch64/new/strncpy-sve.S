/*
 * strcpy/stpcpy - copy a string returning pointer to start/end.
 *
 * Copyright (c) 2018-2024, Arm Limited.
 * SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception
 */

/* Assumptions:
 *
 * ARMv8-a, AArch64, SVE available.
 */

#include "../asmdefs.h"

#ifdef HAVE_SVE

.arch armv8-a+sve

ENTRY (new_strncpy_aarch64_sve)
	PTR_ARG (0)
	PTR_ARG (1)
	SIZE_ARG (2)

	ptrue	p0.b, all		/* all 1s, loop invariant */
	mov	x3, xzr			/* initialize offset */
	cntb	x4			/* initialize stride */

	.p2align 4
	/* Read a vector's worth of bytes.  */
L(loop):
	ld1b	z0.b, p0/z, [x1, x3]
	cmpeq	p1.b, p0/z, z0.b, #0	/* search for zeros */
	b.any	L(end)

	/* No zero found.  Store the whole vector and loop. */
	st1b 	z0.b, p0, [x0, x3]
	add 	x3, x3, x4
	whilelo	p0.b, x3, x2
	b.mi	L(loop)

	/* Found zero or end-of-count.  Crop the vector and return.  */
L(end):
	brka	p0.b, p0/z, p1.b
	st1b	z0.b, p0, [x0, x3]
	ret

END (new_strncpy_aarch64_sve)

#endif
