/*
 * strchr/strchrnul - find a character in a string
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

/* To build as strchrnul, define BUILD_STRCHRNUL before compiling this file.  */
#ifdef BUILD_STRCHRNUL
#define FUNC  new_strchrnul_aarch64_sve
#else
#define FUNC  new_strchr_aarch64_sve
#endif

ENTRY (FUNC)
	PTR_ARG (0)

	dup	z1.b, w1		/* replicate byte across vector */
	ptrue	p1.b			/* all ones; loop invariant */
	cntb	x1

	.p2align 4
	/* Read a vector's worth of bytes, stopping on first fault.  */
L(loop):
	ld1b	z0.b, p1/z, [x0]
	add	x0, x0, x1			/* speculate increment */
	cmpeq	p2.b, p1/z, z0.b, z1.b		/* search for c */
	cmpeq	p3.b, p1/z, z0.b, #0		/* search for 0 */
	orrs	p4.b, p1/z, p2.b, p3.b		/* c | 0 */
	b.none	L(loop)
	sub	x0, x0, x1			/* undo speculate */

	/* Found C or 0.  */
L(found):
	brka	p4.b, p1/z, p4.b	/* find first such */
	sub	x0, x0, #1		/* adjust pointer for that byte */
	incp	x0, p4.b
#ifndef BUILD_STRCHRNUL
	ptest	p4, p2.b		/* was first in c? */
	csel	x0, xzr, x0, none	/* if there was no c, return null */
#endif
L(return):
	ret

END (FUNC)

#endif

