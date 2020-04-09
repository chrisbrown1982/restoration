	.file	"xadd-inc.c"
.globl foo
	.bss
	.align 4
	.type	foo, @object
	.size	foo, 4
foo:
	.zero	4
	.text
.globl my_increment
	.type	my_increment, @function
my_increment:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$1, -4(%rbp)
	movl	-4(%rbp), %eax
#APP
# 8 "xadd-inc.c" 1
	lock ; xaddl %eax,foo(%rip)
	
# 0 "" 2
#NO_APP
	movl	%eax, -8(%rbp)
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	my_increment, .-my_increment
.globl my_lock
	.type	my_lock, @function
my_lock:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	my_lock, .-my_lock
.globl my_unlock
	.type	my_unlock, @function
my_unlock:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	my_unlock, .-my_unlock
	.ident	"GCC: (GNU) 4.4.6 20110731 (Red Hat 4.4.6-3)"
	.section	.note.GNU-stack,"",@progbits
