/*
 * strcmp - compare two strings
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

ENTRY (new_strcmp_aarch64_sve)
	PTR_ARG (0)
	PTR_ARG (1)

	ptrue	p1.b, all		/* all ones; loop invariant */
	mov	x2, xzr			/* initialize offset */
	cntb	x3

	.p2align 4
	/* Read a vector's worth of bytes.  */
L(loop):
	ld1b	z0.b, p1/z, [x0, x2]
	ld1b	z1.b, p1/z, [x1, x2]

	add	x2, x2, x3		/* skip bytes for next round */
	cmpeq	p2.b, p1/z, z0.b, z1.b	/* compare strings */
	cmpne	p3.b, p1/z, z0.b, #0	/* search for ~zero */
	nands	p2.b, p1/z, p2.b, p3.b	/* ~(eq & ~zero) -> ne | zero */
	b.none	L(loop)

	/* Found end-of-string or inequality.  */
L(inequality):
	brkb	p2.b, p1/z, p2.b	/* find first such */
	lasta	w0, p2, z0.b		/* extract each char */
	lasta	w1, p2, z1.b
L(return):
	sub	x0, x0, x1		/* return comparison */
	ret

END (new_strcmp_aarch64_sve)

#endif

