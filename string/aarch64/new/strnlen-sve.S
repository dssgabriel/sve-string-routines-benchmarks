/*
 * strnlen - calculate the length of a string with limit.
 *
 * Copyright (c) 2019-2024, Arm Limited.
 * SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception
 */

/* Assumptions:
 *
 * ARMv8-a, AArch64, SVE available.
 */

#include "../asmdefs.h"

#ifdef HAVE_SVE

.arch armv8-a+sve

ENTRY (new_strnlen_aarch64_sve)
	PTR_ARG (0)
	SIZE_ARG (1)

	mov	x2, xzr			/* initialize len */
	cntb	x3

	.p2align 4
	/* We have off + vl <= max, and so may read the whole vector.  */
L(loop):
	whilelo	p0.b, x2, x1
	ld1b	z0.b, p0/z, [x0, x2]
	cmpeq	p2.b, p0/z, z0.b, #0
	b.any	L(zero)

	add	x2, x2, x3
	b.last	L(loop)

	/* We have off + vl < max.  Test for off == max before proceeding.  */
	b.none	L(end)

	ld1b	z0.b, p0/z, [x0, x2]
	cmpeq	p2.b, p0/z, z0.b, #0

	/* Found end-of-string or zero.  */
L(zero):
	brkb	p2.b, p0/z, p2.b
	incp	x2, p2.b
	mov	x0, x2
	ret

	/* End of count.  Return max.  */
L(end):
	mov	x0, x1
	ret

END (new_strnlen_aarch64_sve)

#endif

