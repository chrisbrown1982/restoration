	.text
.globl _newtq
_newtq:
LFB7:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	subq	$16, %rsp
LCFI2:
	movl	$412, %edi
	call	_malloc
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movb	$0, (%rax)
	movq	-8(%rbp), %rax
	movl	$0, 4(%rax)
	movq	-8(%rbp), %rax
	movl	$0, 8(%rax)
	movq	-8(%rbp), %rax
	leave
	ret
LFE7:
.globl _gettask
_gettask:
LFB8:
	pushq	%rbp
LCFI3:
	movq	%rsp, %rbp
LCFI4:
	subq	$32, %rsp
LCFI5:
	movq	%rdi, -24(%rbp)
L4:
	movq	-24(%rbp), %rdi
	call	_my_lock
	movq	-24(%rbp), %rax
	movl	8(%rax), %edx
	movq	-24(%rbp), %rax
	movl	4(%rax), %eax
	cmpl	%eax, %edx
	jne	L5
	movq	-24(%rbp), %rdi
	call	_my_unlock
L5:
	movq	-24(%rbp), %rax
	movl	8(%rax), %edx
	movq	-24(%rbp), %rax
	movl	4(%rax), %eax
	cmpl	%eax, %edx
	je	L4
	movq	-24(%rbp), %rax
	movl	8(%rax), %eax
	movq	-24(%rbp), %rdx
	cltq
	movl	12(%rdx,%rax,4), %eax
	movl	%eax, -4(%rbp)
	movq	-24(%rbp), %rax
	movl	8(%rax), %eax
	leal	1(%rax), %edx
	movq	-24(%rbp), %rax
	movl	%edx, 8(%rax)
	movq	-24(%rbp), %rax
	movl	8(%rax), %eax
	cmpl	$100, %eax
	jne	L8
	movq	-24(%rbp), %rax
	movl	$0, 8(%rax)
L8:
	movq	-24(%rbp), %rdi
	call	_my_unlock
	movl	-4(%rbp), %eax
	leave
	ret
LFE8:
.globl _puttask
_puttask:
LFB9:
	pushq	%rbp
LCFI6:
	movq	%rsp, %rbp
LCFI7:
	subq	$32, %rsp
LCFI8:
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
L12:
	movq	-24(%rbp), %rdi
	call	_my_lock
	movq	-24(%rbp), %rax
	movl	4(%rax), %edx
	movq	-24(%rbp), %rax
	movl	8(%rax), %eax
	decl	%eax
	cmpl	%eax, %edx
	jne	L13
	movq	-24(%rbp), %rdi
	call	_my_unlock
L13:
	movq	-24(%rbp), %rax
	movl	4(%rax), %edx
	movq	-24(%rbp), %rax
	movl	8(%rax), %eax
	decl	%eax
	cmpl	%eax, %edx
	je	L12
	movq	-24(%rbp), %rax
	movl	4(%rax), %ecx
	movq	-24(%rbp), %rsi
	movslq	%ecx,%rdx
	movl	-28(%rbp), %eax
	movl	%eax, 12(%rsi,%rdx,4)
	leal	1(%rcx), %edx
	movq	-24(%rbp), %rax
	movl	%edx, 4(%rax)
	movq	-24(%rbp), %rax
	movl	4(%rax), %eax
	cmpl	$100, %eax
	jne	L16
	movq	-24(%rbp), %rax
	movl	$0, 4(%rax)
L16:
	movq	-24(%rbp), %rdi
	call	_my_unlock
	leave
	ret
LFE9:
.globl _worker
_worker:
LFB10:
	pushq	%rbp
LCFI9:
	movq	%rsp, %rbp
LCFI10:
	subq	$32, %rsp
LCFI11:
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	call	_pthread_self
	movq	%rax, -16(%rbp)
	movq	-24(%rbp), %rdi
	call	_gettask
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %edi
	call	_payload
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %esi
	movq	-32(%rbp), %rdi
	call	_puttask
	leave
	ret
LFE10:
.globl _sworker
_sworker:
LFB11:
	pushq	%rbp
LCFI12:
	movq	%rsp, %rbp
LCFI13:
	subq	$16, %rsp
LCFI14:
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
L22:
	movq	-16(%rbp), %rsi
	movq	-8(%rbp), %rdi
	call	_worker
	jmp	L22
LFE11:
	.cstring
	.align 3
LC0:
	.ascii "ERROR; return code from pthread_create() is %d\12\0"
	.text
.globl _makeThread
_makeThread:
LFB12:
	pushq	%rbp
LCFI15:
	movq	%rsp, %rbp
LCFI16:
	subq	$32, %rsp
LCFI17:
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movl	$8, %edi
	call	_malloc
	movq	%rax, -16(%rbp)
	movq	-32(%rbp), %rcx
	movq	-24(%rbp), %rdx
	movq	-16(%rbp), %rdi
	movl	$0, %esi
	call	_pthread_create
	movl	%eax, -4(%rbp)
	cmpl	$0, -4(%rbp)
	je	L27
	movl	-4(%rbp), %esi
	leaq	LC0(%rip), %rdi
	movl	$0, %eax
	call	_printf
	movl	$-1, %edi
	call	_exit
L27:
	leave
	ret
LFE12:
.globl _createfarm
_createfarm:
LFB13:
	pushq	%rbp
LCFI18:
	movq	%rsp, %rbp
LCFI19:
	subq	$32, %rsp
LCFI20:
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movq	$0, -16(%rbp)
	jmp	L29
L30:
	movq	-16(%rbp), %rsi
	movq	-24(%rbp), %rdi
	call	_makeThread
	incq	-16(%rbp)
L29:
	movl	-28(%rbp), %eax
	cltq
	cmpq	-16(%rbp), %rax
	jg	L30
	leave
	ret
LFE13:
.globl _createpipe
_createpipe:
LFB14:
	pushq	%rbp
LCFI21:
	movq	%rsp, %rbp
LCFI22:
	subq	$16, %rsp
LCFI23:
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rdi
	movl	$0, %esi
	call	_makeThread
	movq	-16(%rbp), %rdi
	movl	$1, %esi
	call	_makeThread
	leave
	ret
LFE14:
	.section __TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame1:
	.set L$set$0,LECIE1-LSCIE1
	.long L$set$0
LSCIE1:
	.long	0x0
	.byte	0x1
	.ascii "zR\0"
	.byte	0x1
	.byte	0x78
	.byte	0x10
	.byte	0x1
	.byte	0x10
	.byte	0xc
	.byte	0x7
	.byte	0x8
	.byte	0x90
	.byte	0x1
	.align 3
LECIE1:
.globl _newtq.eh
_newtq.eh:
LSFDE1:
	.set L$set$1,LEFDE1-LASFDE1
	.long L$set$1
LASFDE1:
	.long	LASFDE1-EH_frame1
	.quad	LFB7-.
	.set L$set$2,LFE7-LFB7
	.quad L$set$2
	.byte	0x0
	.byte	0x4
	.set L$set$3,LCFI0-LFB7
	.long L$set$3
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$4,LCFI1-LCFI0
	.long L$set$4
	.byte	0xd
	.byte	0x6
	.align 3
LEFDE1:
.globl _gettask.eh
_gettask.eh:
LSFDE3:
	.set L$set$5,LEFDE3-LASFDE3
	.long L$set$5
LASFDE3:
	.long	LASFDE3-EH_frame1
	.quad	LFB8-.
	.set L$set$6,LFE8-LFB8
	.quad L$set$6
	.byte	0x0
	.byte	0x4
	.set L$set$7,LCFI3-LFB8
	.long L$set$7
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$8,LCFI4-LCFI3
	.long L$set$8
	.byte	0xd
	.byte	0x6
	.align 3
LEFDE3:
.globl _puttask.eh
_puttask.eh:
LSFDE5:
	.set L$set$9,LEFDE5-LASFDE5
	.long L$set$9
LASFDE5:
	.long	LASFDE5-EH_frame1
	.quad	LFB9-.
	.set L$set$10,LFE9-LFB9
	.quad L$set$10
	.byte	0x0
	.byte	0x4
	.set L$set$11,LCFI6-LFB9
	.long L$set$11
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$12,LCFI7-LCFI6
	.long L$set$12
	.byte	0xd
	.byte	0x6
	.align 3
LEFDE5:
.globl _worker.eh
_worker.eh:
LSFDE7:
	.set L$set$13,LEFDE7-LASFDE7
	.long L$set$13
LASFDE7:
	.long	LASFDE7-EH_frame1
	.quad	LFB10-.
	.set L$set$14,LFE10-LFB10
	.quad L$set$14
	.byte	0x0
	.byte	0x4
	.set L$set$15,LCFI9-LFB10
	.long L$set$15
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$16,LCFI10-LCFI9
	.long L$set$16
	.byte	0xd
	.byte	0x6
	.align 3
LEFDE7:
.globl _sworker.eh
_sworker.eh:
LSFDE9:
	.set L$set$17,LEFDE9-LASFDE9
	.long L$set$17
LASFDE9:
	.long	LASFDE9-EH_frame1
	.quad	LFB11-.
	.set L$set$18,LFE11-LFB11
	.quad L$set$18
	.byte	0x0
	.byte	0x4
	.set L$set$19,LCFI12-LFB11
	.long L$set$19
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$20,LCFI13-LCFI12
	.long L$set$20
	.byte	0xd
	.byte	0x6
	.align 3
LEFDE9:
.globl _makeThread.eh
_makeThread.eh:
LSFDE11:
	.set L$set$21,LEFDE11-LASFDE11
	.long L$set$21
LASFDE11:
	.long	LASFDE11-EH_frame1
	.quad	LFB12-.
	.set L$set$22,LFE12-LFB12
	.quad L$set$22
	.byte	0x0
	.byte	0x4
	.set L$set$23,LCFI15-LFB12
	.long L$set$23
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$24,LCFI16-LCFI15
	.long L$set$24
	.byte	0xd
	.byte	0x6
	.align 3
LEFDE11:
.globl _createfarm.eh
_createfarm.eh:
LSFDE13:
	.set L$set$25,LEFDE13-LASFDE13
	.long L$set$25
LASFDE13:
	.long	LASFDE13-EH_frame1
	.quad	LFB13-.
	.set L$set$26,LFE13-LFB13
	.quad L$set$26
	.byte	0x0
	.byte	0x4
	.set L$set$27,LCFI18-LFB13
	.long L$set$27
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$28,LCFI19-LCFI18
	.long L$set$28
	.byte	0xd
	.byte	0x6
	.align 3
LEFDE13:
.globl _createpipe.eh
_createpipe.eh:
LSFDE15:
	.set L$set$29,LEFDE15-LASFDE15
	.long L$set$29
LASFDE15:
	.long	LASFDE15-EH_frame1
	.quad	LFB14-.
	.set L$set$30,LFE14-LFB14
	.quad L$set$30
	.byte	0x0
	.byte	0x4
	.set L$set$31,LCFI21-LFB14
	.long L$set$31
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$32,LCFI22-LCFI21
	.long L$set$32
	.byte	0xd
	.byte	0x6
	.align 3
LEFDE15:
	.subsections_via_symbols
