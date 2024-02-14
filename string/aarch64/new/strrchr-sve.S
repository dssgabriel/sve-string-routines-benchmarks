/*
 * strrchr - find the last of a character in a string
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

ENTRY (new_strrchr_aarch64_sve)
	PTR_ARG (0)

	dup	z1.b, w1		/* replicate byte across vector */
	ptrue	p1.b			/* all ones; loop invariant */
	mov	x2, xzr			/* no match found so far */
	pfalse	p2.b
	cntb	x3

	.p2align 4
	/* Read a vector's worth of bytes, stopping on first fault.  */
L(loop):
	ld1b	z0.b, p1/z, [x0]
	add	x0, x0, x3		/* skip bytes this round */
	cmpeq	p3.b, p1/z, z0.b, #0	/* search for 0 */
	b.any	L(end)

	cmpeq	p3.b, p1/z, z0.b, z1.b	/* search for c; no eos */
	b.none	L(loop)

	mov	x2, x0			/* save advanced base */
	mov	p2.b, p3.b		/* save current search */
	b	L(loop)

	/* Found end-of-string.  */
	add	x0, x0, x3		/* advance base */
L(end):
	brka	p3.b, p1/z, p3.b	/* mask after first 0 */
	cmpeq	p3.b, p3/z, z0.b, z1.b	/* search for c not after eos */
	b.any	L(last_match)

	/* No C within last vector.  Did we have one before?  */
	cbz	x2, L(return)
	mov	x0, x2			/* restore advanced base */
	mov	p3.b, p2.b		/* restore saved search */

	/* Find the *last* match in the predicate.  This is slightly
	   more complicated than finding the first match.  */
L(last_match):
	rev	p3.b, p3.b		/* reverse the bits */
	brka	p3.b, p1/z, p3.b	/* find position of last match */
	decp	x0, p3.b		/* retard pointer to last match */
	ret

	/* No C whatsoever.  Return NULL.  */
L(return):
	mov	x0, #0
	ret

END (new_strrchr_aarch64_sve)

#endif

