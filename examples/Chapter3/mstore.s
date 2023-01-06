	.file	"mstore.c"
	.text
	.globl	multstore
	.type	multstore, @function
multstore:
.LFB0:
	.cfi_startproc
	endbr64
	# push data(q: length = 64 bits) of rbx to stack
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	# move data of rdx to rbx
	movq	%rdx, %rbx
	# call mult2
	call	mult2@PLT
	# move data of rax to rbx
	movq	%rax, (%rbx)
	# pop pop of stack to rbx
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE0:
	.size	multstore, .-multstore
	.ident	"GCC: (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
