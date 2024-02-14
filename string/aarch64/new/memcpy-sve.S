/*
 * memcpy - copy memory area
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

ENTRY (new_memcpy_aarch64_sve)
	PTR_ARG (0)
	PTR_ARG (1)
	SIZE_ARG (2)

	ptrue	p0.b
	mov	x3, xzr			/* initialize off */
	cntb	x4

	/* Loop entry.  */
L(loop):
	ld1b	z0.b, p0/z, [x1, x3]	/* read vectors bounded by max.  */
	add	x3, x3, x4
	whilelo	p0.b, x3, x2		/* while off < max */
	st1b	z0.b, p0, [x0, x3]	/* store vectors bounded by max.  */
	b.mi    L(loop)
L(return):
	ret

END (new_memcpy_aarch64_sve)

#endif

