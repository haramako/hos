#include "smp_trampoline.h"

/* Note that .data section cannot be used in the trampoline file because we
   need to determine the contiguous space of the trampoline code within a
   section from two labels; between _trampoline and _trampoline_end. */

	.globl	_trampoline
	.globl	_trampoline_end

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

	movl %cr0, %eax                                  // Set the A-register to control register 0.
    andl $0x7fff, %eax     // Clear the PG-bit, which is bit 31.
    movl %eax, %cr0                                  // Set control register 0 to the A-register.

	movl $0x7fe3c000, %eax
	movl %eax, %cr3

    movl %cr4, %eax                 // Set the A-register to control register 4.
    orl $(1<<5), %eax              // Set the PAE-bit, which is the 6th bit (bit 5).
    movl %eax, %cr4                 // Set control register 4 to the A-register.
	
    movl $0xC0000080, %ecx          // Set the C-register to 0xC0000080, which is the EFER MSR.
    rdmsr                        // Read from the model-specific register.
    orl $(1 << 8), %eax               // Set the LM-bit which is the 9th bit (bit 8).
    wrmsr                        // Write to the model-specific register.

    movl %cr0, %eax                 // Set the A-register to control register 0.
    orl $(1 << 31 | 1 << 0), %eax     // Set the PG-bit, which is the 31nd bit, and the PM-bit, which is the 0th bit.
    movl %eax, %cr0                 // Set control register 0 to the A-register.
	
	//ljmpl	$AP_GDT_CODE32_SEL,$(ap_entry32 - KERNEL_RELOCBASE)
	//ljmpl	$AP_GDT_CODE32_SEL,$(ap_entry32 - _trampoline + (TRAMPOLINE_VEC << 12))
	ljmpl	$AP_GDT_CODE64_SEL,$(ap_entry64 - _trampoline + (TRAMPOLINE_VEC << 12))

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
	.word	0xffff,0x0,0x9a00,0xaf	/* Code 64 */
	.word	0xffff,0x0,0x9200,0x8f	/* Data 16 */
gdt.1:
gdtr:
	.word	gdt.1 - gdt - 1	/* Limit */
	.long	gdt- _trampoline + (TRAMPOLINE_VEC << 12)/* Base address */



	.align	16
	.code32
ap_entry32:
	mov $0x2f8, %dx
	mov $0x43, %ax
	outb %al, %dx
	.loop: jmp .loop

	.align	16
	.code64
ap_entry64:
	mov $0x43, %ax
	mov $0x2f8, %dx
	outb %al, %dx
	//.loop2: jmp .loop2
	movq 0x8000, %rdi
	movq 0x8008, %rsp
	jmp *%rdi

_trampoline_end:
