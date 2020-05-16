#include "smp_trampoline.h"

/* Note that .data section cannot be used in the trampoline file because we
   need to determine the contiguous space of the trampoline code within a
   section from two labels; between _trampoline and _trampoline_end. */

	.globl	_trampoline
	.globl	_trampoline_end
	.globl	ap_entry32

	.text
	.code16

/*
 * Trampoline code starts.  Note that the trampoline code is loaded into a
 * 4 KiB (aligned) page in the lower 1 MiB of memory.  The %cs is automatically
 * set after the SIPI.  %ip is expected to be zero but not sure.  So, we first
 * calculate the offsets of idtr and gdtr.
 */
_trampoline:
	cli

	mov $0x2f8, %dx
	mov $0x42, %ax
	outb %al, %dx
	
	/* Calculate the base address */
	xorl	%eax,%eax
	movw	$(TRAMPOLINE_VEC << 8),%ax
	movw	%ax,%ds

	/* Setup GDT and IDT */
	lidt	%ds:(idtr - _trampoline)
	lgdt	%ds:(gdtr - _trampoline)

	/* Turn on protected mode */
	movl	%cr0,%eax
	orl	$0x1,%eax
	movl	%eax,%cr0
	//ljmpl	$AP_GDT_CODE32_SEL,$(ap_entry32 - KERNEL_RELOCBASE)
	ljmpl	$AP_GDT_CODE32_SEL,$(ap_entry32 - _trampoline + (TRAMPOLINE_VEC << 12))
	//ljmpl	$AP_GDT_CODE64_SEL,$(ap_entry64 - _trampoline + (TRAMPOLINE_VEC << 12))

	/* Data section but trampoline code cannot be in separated sections */
	.align	16

/* Pseudo interrupt descriptor table */
idtr:
	.word	0x0		/* Limit */
	.long	0x0		/* Base address */
/* Temporary global descriptor table */
gdt:
	.word	0x0,0x0,0x0,0x0		/* Null descriptor */
	.word	0xffff,0x0,0x9a00,0xaf	/* Code 64 */
	.word	0xffff,0x0,0x9200,0xaf	/* Data 64 */
	.word	0xffff,0x0,0x9a00,0xcf	/* Code 32 */
	.word	0xffff,0x0,0x9200,0xcf	/* Data 32 */
	.word	0xffff,0x0,0x9a00,0x8f	/* Code 16 */
	.word	0xffff,0x0,0x9200,0x8f	/* Data 16 */
gdt.1:
gdtr:
	.word	gdt.1 - gdt - 1	/* Limit */
	.long	gdt- _trampoline + (TRAMPOLINE_VEC << 12)/* Base address */



	.code32
ap_entry32:
	mov $0x2f8, %dx
	mov $0x43, %ax
	outb %al, %dx
	.loop: jmp .loop

	.code64
ap_entry64:
	mov $0x2f8, %dx
	mov $0x43, %ax
	outb %al, %dx
	.loop2: jmp .loop2

_trampoline_end:
