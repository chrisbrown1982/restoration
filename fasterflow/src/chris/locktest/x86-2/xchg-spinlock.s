	.file	"xchg-spinlock.c"
.globl lock_word
	.bss
	.type	lock_word, @object
	.size	lock_word, 1
lock_word:
	.zero	1
	.text
.globl my_lock
	.type	my_lock, @function
my_lock:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -20(%rbp)
	movb	$1, -1(%rbp)
.L2:
	movzbl	-1(%rbp), %eax
#APP
# 9 "xchg-spinlock.c" 1
	lock; xchgb  %al,lock_word(%rip)
	
# 0 "" 2
#NO_APP
	movb	%al, -2(%rbp)
	cmpb	$1, -2(%rbp)
	je	.L2
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	my_lock, .-my_lock
.globl my_unlock
	.type	my_unlock, @function
my_unlock:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movb	$0, lock_word(%rip)
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	my_unlock, .-my_unlock
	.ident	"GCC: (GNU) 4.4.6 20110731 (Red Hat 4.4.6-3)"
	.section	.note.GNU-stack,"",@progbits
