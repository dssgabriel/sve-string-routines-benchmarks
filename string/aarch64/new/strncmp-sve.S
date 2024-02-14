/*
 * strncmp - compare two strings with limit
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

ENTRY (new_strncmp_aarch64_sve)
	PTR_ARG (0)
	PTR_ARG (1)
	SIZE_ARG (2)

	mov	x3, xzr			/* initialize off */
	cntb	x4

	/* Loop entry.  */
L(loop):
	whilelo	p0.b, x3, x2		/* while off < max */
	b.none	L(end)

	ld1b	z0.b, p0/z, [x0, x3]
	ld1b	z1.b, p0/z, [x1, x3]

	/* Increment for a whole vector, even if we've only read a partial.
	   Using ADD is significantly cheaper than INCB or INCP, and since OFF
	   is not used after the loop it is ok to increment OFF past MAX.  */
	add	x3, x3, x4
	cmpeq	p1.b, p0/z, z0.b, z1.b	/* compare strings */
	cmpne	p2.b, p0/z, z0.b, #0	/* search for ~zero */
	nands	p2.b, p0/z, p1.b, p2.b	/* ~(eq & ~zero) -> ne | zero */
	b.none	L(loop)

	/* Found end-of-string or inequality.  */
L(inequality):
	brkb	p2.b, p0/z, p2.b	/* find first such */
	lasta	w0, p2, z0.b		/* extract each char */
	lasta	w1, p2, z1.b
	sub	x0, x0, x1		/* return comparison */
	ret

	/* Found end-of-count.  */
L(end):
	mov	x0, 0			/* return equal */
	ret

END (new_strncmp_aarch64_sve)

#endif

