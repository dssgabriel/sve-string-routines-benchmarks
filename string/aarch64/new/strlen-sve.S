/*
 * strlen - compute the length of a string
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

ENTRY (new_strlen_aarch64_sve)
	PTR_ARG (0)

	ptrue	p2.b		/* all ones; loop invariant */
	mov	x1, xzr		/* initialize length */
	cntb	x2

	.p2align 4
	/* Read a vector's worth of bytes.  */
L(loop):
	ld1b	z0.b, p2/z, [x0, x1]
	add	x1, x1, x2		/* speculate increment */
	cmpeq	p1.b, p2/z, z0.b, #0	/* search for 0 */
	b.none	L(loop)

	/* Zero found.  Select the bytes before the first and count them.  */
	sub	x1, x1, x2		/* undo speculate */
L(zero):
	brkb	p0.b, p2/z, p1.b
	incp	x1, p0.b
	mov	x0, x1			/* return count */
L(return):
	ret

END (new_strlen_aarch64_sve)

#endif

